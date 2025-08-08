#pragma once
#include <stdint.h>
#include "HardwareSerial.h"
#include "Constants.h"

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
        uint8_t bufferSize = config::EVENTQSIZE;
        uint8_t eventsInBuffer = 0;
        Event events[config::EVENTQSIZE];

public:
	void addEvent(Event);
	Event getEvent();
	void clear();
};