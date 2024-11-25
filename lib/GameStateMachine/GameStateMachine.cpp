#include "GameStateMachine.h"

void GameStateMachine::init(ComStateMachineMaster *comStateMachine, LedStateMachine *ledStateMachine, EventBroker *eventBroker)
{
	this->comStateMachine = comStateMachine;
	this->ledStateMachine = ledStateMachine;
	this->eventBroker = eventBroker;
}

bool GameStateMachine::turnDefined(uint8_t deviceId)
{
	for (int i = 0; i < numberOfPlayers; i++)
	{
		if (turnOrder[i] == deviceId)
		{
			return true;
		}
	}
	return false;
}

int GameStateMachine::playerIndex(int deviceId)
{
	for (int i = 0; i < numberOfPlayers; i++)
	{
		if (turnOrder[i] == deviceId)
		{
			return i;
		}
	}
	return -1;
}

LedState GameStateMachine::payloadToState(PayloadType payloadType)
{
	switch (payloadType)
	{
	case PayloadType::LEDOFF:
		return LedState::OFF;
		break;
	case PayloadType::LEDON:
		return LedState::ON;
		break;
	case PayloadType::REORDER:
		return LedState::REORDER;
	}
}

bool GameStateMachine::sendToDevice(uint8_t deviceId, PayloadType payloadType)
{
	if (deviceId == 0)
	{
		ledStateMachine->setState(payloadToState(payloadType));
		return true;
	}
	else
	{
		comStateMachine->setPayloadType(payloadType);
		bool status = comStateMachine->send(deviceId - 1);
		if (!status)
		{
			comStateMachine->setState(ComStateMaster::SETUP_PAIRING);
			gameState = GameState::SETUP;
			return false;
		}
		return true;
	}
}

void GameStateMachine::sendAll(PayloadType payloadType)
{
	ledStateMachine->setState(payloadToState(payloadType));
	for (int i = 0; i < (numberOfPlayers - 1); i++)
	{
		comStateMachine->setPayloadType(payloadType);
		comStateMachine->send(i);
	}
}

void GameStateMachine::update()
{
	switch (gameState)
	{
	case GameState::SETUP:
	{
		Event event = eventBroker->getEvent();
		if (event.eventType == EventType::SHORT && event.deviceId == 0)
		{
			gameState = GameState::PLAY;
			Serial.println("GameState -> PLAY");
			numberOfPlayers = comStateMachine->getNumberDevices();
			sendAll(PayloadType::LEDOFF);
			sendToDevice(turnOrder[currentPlayerIndex], PayloadType::LEDON);
		}
		break;
	}
	case GameState::PLAY:
	{
		Event event = eventBroker->getEvent();
		if (event.eventType == EventType::SHORT && event.deviceId == turnOrder[currentPlayerIndex])
		{
			Serial.println("Passing light");
			uint8_t nextPlayer = turnOrder[(currentPlayerIndex + 1) % numberOfPlayers];
			uint8_t nextPlayerIndex = (currentPlayerIndex + 1) % numberOfPlayers;
			sendToDevice(turnOrder[currentPlayerIndex], PayloadType::LEDOFF);
			sendToDevice(nextPlayer, PayloadType::LEDON);
			currentPlayerIndex = nextPlayerIndex;
		}
		else if (event.eventType == EventType::LONG && event.deviceId != turnOrder[currentPlayerIndex])
		{
			Serial.println("Stealing light");
			sendToDevice(turnOrder[currentPlayerIndex], PayloadType::LEDOFF);
			sendToDevice(event.deviceId, PayloadType::LEDON);
			currentPlayerIndex = playerIndex(event.deviceId);
		}
		else if (event.eventType == EventType::DOUBLE)
		{
			Serial.println("Reordering");
			reorderCount = 0;
			gameState = GameState::REORDER;
			sendAll(PayloadType::REORDER);
			for (int i = 0; i < numberOfPlayers; i++)
			{
				turnOrder[i] = -1;
			}
		}
		break;
	}
	case GameState::REORDER:
	{
		Event event = eventBroker->getEvent();
		if (reorderCount >= numberOfPlayers)
		{
			gameState = GameState::PLAY;
			currentPlayerIndex = 0;
			sendToDevice(turnOrder[currentPlayerIndex], PayloadType::LEDON);
		}
		if (event.eventType == EventType::SHORT && !turnDefined(event.deviceId))
		{
			sendToDevice(event.deviceId, PayloadType::LEDOFF);
			turnOrder[reorderCount] = event.deviceId;
			reorderCount++;
		}
	}
	}
}