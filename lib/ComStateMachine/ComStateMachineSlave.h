#pragma once
#include <SPI.h>
#include <ESPNowW.h>
#include "LedStateMachine.h"
#include "EventBroker.h"
#include "HardwareSerial.h"
#include <MacAddress.h>

#include "ComStateMachineI.h"

#define RETRY_DELAY 500
#define ACK_TIMEOUT 2000

enum class ComStateSlave
{
	STANDBY,
	RADIO_UNRESPONSIVE,
	SETUP_PAIRING,
	PAIRING,
	WAITING_ADRS,
	SETUP_LISTENING,
	LISTENING,
	CONNECTED,
};

class ComStateMachineSlave : public ComStateMachineI
{
private:
	ComStateSlave comState = ComStateSlave::STANDBY;
	EventBroker *eventBroker;
	MacAddress server;

public:
	void init(LedStateMachine *, EventBroker *);
	void update();
	int send();
	void setState(ComStateSlave);
	void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len);
	void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus);
};