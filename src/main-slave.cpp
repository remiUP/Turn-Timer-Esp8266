#include <Arduino.h>
#include <ESPNowW.h>
#include <esp8266wifi.h>
#include "LedStateMachine.h"
#include "ComStateMachineSlave.h"
#include "EventBroker.h"
#include <Bounce2.h>

LedStateMachine ledStateMachine;

Bounce button = Bounce();

EventBroker eventBroker;
ComStateMachineSlave comStateMachineSlave;

void setup()
{
	// Init Serial Monitor
	Serial.begin(9600);
	pinMode(LED_BUILTIN, OUTPUT);

	// Set device as a Wi-Fi Station
	WiFi.mode(WIFI_STA);
	// print mac address
	Serial.print("MAC Address: ");
	Serial.println(WiFi.macAddress());

	ledStateMachine.init();

	button.attach(D1, INPUT_PULLUP);
	button.interval(50);

	comStateMachineSlave.init(&ledStateMachine, &eventBroker);

	// Once ESPNow is successfully Init, we will register for Send CB to
	// get the status of Trasnmitted packet
	esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
}

void loop()
{

	if (button.fell())
	{
		Event event;
		event.eventType = EventType::SHORT;
		eventBroker.addEvent(event);
	}

	ledStateMachine.update();
	comStateMachineSlave.update();
	button.update();
}