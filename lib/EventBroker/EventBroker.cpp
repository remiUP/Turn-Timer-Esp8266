#include "EventBroker.h"

Event EventBroker::getEvent()
{
	if (eventsInBuffer > 0)
	{
		Serial.println("Get event");
		Serial.print("Device ID : ");
		Serial.print(events[eventsInBuffer - 1].deviceId);
		Serial.print(" | Event type : ");
		Serial.println((int)events[eventsInBuffer - 1].eventType);
		return events[--eventsInBuffer];
	}
	Event noneEvent;
	noneEvent.deviceId = 0;
	noneEvent.eventType = EventType::NONE;
	return noneEvent;
}

void EventBroker::addEvent(Event event)
{

	if (eventsInBuffer < bufferSize)
	{
		Serial.println("Event added");
		Serial.print("Device ID : ");
		Serial.print(event.deviceId);
		Serial.print(" | Event type : ");
		Serial.println((int)event.eventType);
		events[eventsInBuffer] = event;
		eventsInBuffer++;
		Serial.print("Elements in buffer : ");
		Serial.println(eventsInBuffer);
	}
	else
	{
		Serial.println("Buffer full !");
	}
}

void EventBroker::clear()
{
	eventsInBuffer = 0;
}