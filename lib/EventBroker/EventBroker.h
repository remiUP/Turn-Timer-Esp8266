#pragma once
#include <stdint.h>
#include "HardwareSerial.h"

#define EVENTQSIZE 5

enum class EventType
{
	NONE,
	SHORT,
	LONG,
	DOUBLE
};

struct Event
{
	EventType eventType;
	uint8_t deviceId;
};

class EventBroker
{
private:
	uint8_t bufferSize = EVENTQSIZE;
	uint8_t eventsInBuffer = 0;
	Event events[EVENTQSIZE];

public:
	void addEvent(Event);
	Event getEvent();
	void clear();
};