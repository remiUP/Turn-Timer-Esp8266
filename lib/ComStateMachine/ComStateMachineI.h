#pragma once
#include "LedStateMachine.h"
#include <ESPNowW.h>
#include <MacAddress.h>

enum PayloadType
{
	NONE,	 // No payload
	CRQST,	 // Connection Request
	APP,	 // Assign Pipe
	BPRESS,	 // Button press
	BLPRESS, // Button long press
	LEDT,	 // Led Toggle
	LEDON,	 // Led on
	LEDOFF,	 // Led off
	REORDER, // Reorder light
};

struct Payload
{
	PayloadType payloadType;
	uint32_t data;
};

#include <MacAddress.h>

struct ReceivedPayload
{
	PayloadType payloadType;
	MacAddress macAddress;
	uint32_t data;
};

class ComStateMachineI
{
protected:
	uint8_t role;
	bool payloadAvailable = false;
	Payload payload;
	ReceivedPayload receivedPayload;
	uint16_t previous;
	LedStateMachine *ledStateMachine;
	ReceivedPayload getReceivedPayload();

public:
	virtual void update() = 0;
	virtual int send(int) = 0;
	virtual void init();
	void setPayloadType(PayloadType);
	void setPayloadData(uint32_t);
};