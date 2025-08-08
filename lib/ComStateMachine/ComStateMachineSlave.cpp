#pragma once
#include "ComStateMachineSlave.h"

void ComStateMachineSlave::init(LedStateMachine *ledStateMachine, EventBroker *eventBroker)
{
	this->ledStateMachine = ledStateMachine;
	this->eventBroker = eventBroker;
	server.setMacAddressBroadcast();

	if (esp_now_init() != 0)
	{
		comState = ComStateSlave::RADIO_UNRESPONSIVE;
		Serial.println("Unable to connect radio");
		this->ledStateMachine->setState(LedState::ERROR);
		return;
	}
	Serial.println("Radio connected");
	this->comState = ComStateSlave::SETUP_PAIRING;

	static auto OnRecvCb = [this](uint8_t *mac, uint8_t *incomingData, uint8_t len)
	{ this->OnDataRecv(mac, incomingData, len); };
	esp_now_register_recv_cb([](uint8_t *mac, uint8_t *incomingData, uint8_t len)
							 { OnRecvCb(mac, incomingData, len); });

	static auto OnSendCb = [this](uint8_t *mac_addr, uint8_t sendStatus)
	{ this->OnDataSent(mac_addr, sendStatus); };
	esp_now_register_send_cb([](uint8_t *mac_addr, uint8_t sendStatus)
							 { OnSendCb(mac_addr, sendStatus); });
}

void ComStateMachineSlave::OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
	Payload incomingPayload;
	memcpy(&incomingPayload, incomingData, sizeof(incomingPayload));
	receivedPayload.macAddress.setMacAddress(mac);
	receivedPayload.payloadType = incomingPayload.payloadType;
	receivedPayload.data = incomingPayload.data;
	payloadAvailable = true;
	Serial.print("Bytes received: ");
	Serial.println(len);
	Serial.print("Status: ");
	Serial.println(payload.payloadType);
	Serial.print("Mac: ");
	Serial.println(receivedPayload.macAddress);
	Serial.print("Data: ");
	Serial.println(payload.data);
}

void ComStateMachineSlave::OnDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
	Serial.print("Last Packet Send Status: ");
	if (sendStatus == 0)
	{
		Serial.println("Delivery success");
	}
	else
	{
		Serial.println("Delivery fail");
	}
}

void ComStateMachineSlave::setState(ComStateSlave state)
{
	this->comState = state;
};

int ComStateMachineSlave::send()
{
	int status = esp_now_send(server.getMacAddress(), (uint8_t *)&payload, sizeof(payload));
	comState = ComStateSlave::SETUP_LISTENING;
	return status;
};

void ComStateMachineSlave::update()
{
	Event newEvent = this->eventBroker->getEvent();
	ReceivedPayload receivedPayload = getReceivedPayload();
	switch (comState)
	{
	case ComStateSlave::STANDBY:
		break;

	case ComStateSlave::SETUP_PAIRING:
	{
		payload.payloadType = PayloadType::CRQST;
		comState = ComStateSlave::PAIRING;
		ledStateMachine->setState(LedState::PAIRING);
		break;
	}
	case ComStateSlave::PAIRING:
	{
                if (millis() - previous > config::RETRY_DELAY)
		{
			Serial.println(millis());
			if (send() == 0)
			{
				Serial.println("Connexion request successfully sent");
				comState = ComStateSlave::WAITING_ADRS;
			}
			else
			{
				Serial.println("Connexion request not received, retrying...");
			}
			previous = millis();
		}
		break;
	}
	case ComStateSlave::WAITING_ADRS:
	{
		if (payload.payloadType == PayloadType::APP)
		{
			Serial.println("Received APP");
			Serial.print("New role is : ");
			Serial.println(payload.data);
			comState = ComStateSlave::SETUP_LISTENING;
			ledStateMachine->setState(LedState::PAIRED);
			server.setMacAddress(receivedPayload.macAddress);
			esp_now_add_peer(receivedPayload.macAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
		}
                else if (millis() - previous > config::ACK_TIMEOUT)
		{
			Serial.println(millis() - previous);
			Serial.println("APP timed out");
			comState = ComStateSlave::SETUP_PAIRING;
		}
		break;
	}
	case ComStateSlave::SETUP_LISTENING:
	{
		Serial.println("Com setup");
		this->comState = ComStateSlave::LISTENING;
		Serial.println("Listening");
		break;
	}
	case ComStateSlave::LISTENING:
	{
		if (newEvent.eventType == EventType::SHORT)
		{
			Serial.println("Sending SHORT");
			payload.payloadType = PayloadType::BPRESS;
			send();
			break;
		}
		else if (newEvent.eventType == EventType::LONG)
		{
			Serial.println("Sending LONG");
			payload.payloadType = PayloadType::BLPRESS;
			send();
			break;
		}
		else if (newEvent.eventType == EventType::DOUBLE)
		{
			Serial.println("Sending REORDER");
			payload.payloadType = PayloadType::REORDER;
			send();
			break;
		}
		if (receivedPayload.payloadType == PayloadType::NONE)
		{
			break;
		}
		Serial.println("Incoming payload");
		Serial.print("Payload type :");
		Serial.println(payload.payloadType);
		if (payload.payloadType == PayloadType::LEDON)
		{
			this->ledStateMachine->setState(LedState::ON);
			Serial.println("LED ON");
		}
		else if (payload.payloadType == PayloadType::LEDOFF)
		{
			this->ledStateMachine->setState(LedState::OFF);
			Serial.println("LED OFF");
		}
		else if (payload.payloadType == PayloadType::REORDER)
		{
			this->ledStateMachine->setState(LedState::REORDER);
			Serial.println("LED REORDER");
		}
		break;
	}
	default:
		break;
	}
}