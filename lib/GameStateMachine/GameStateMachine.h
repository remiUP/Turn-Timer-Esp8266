#pragma once
#include <stdint.h>
#include "LedStateMachine.h"
#include "EventBroker.h"
#include "ComStateMachineMaster.h"

enum class GameState
{
	SETUP,
	PLAY,
	REORDER,
};

class GameStateMachine
{
private:
	GameState gameState = GameState::SETUP;
	uint8_t numberOfPlayers = 0;
	uint8_t currentPlayerIndex = 0;
	int turnOrder[6] = {0, 1, 2, 3, 4, 5};
	uint8_t reorderCount = 0;
	ComStateMachineMaster *comStateMachine;
	LedStateMachine *ledStateMachine;
	EventBroker *eventBroker;
	bool turnDefined(uint8_t);
	int playerIndex(int);
	LedState payloadToState(PayloadType);

public:
	void init(ComStateMachineMaster *, LedStateMachine *, EventBroker *);
	void update();
	bool sendToDevice(uint8_t, PayloadType);
	void sendAll(PayloadType);
};