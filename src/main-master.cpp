#include <Arduino.h>
#include <ESPNowW.h>
#include <esp8266wifi.h>
#include "LedStateMachine.h"
#include "ComStateMachineMaster.h"
#include "ScreenStateMachine.h"
#include "EventBroker.h"
#include <Bounce2.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Constants.h"

ScreenStateMachine screenStateMachine;
LedStateMachine ledStateMachine;

Bounce button = Bounce();

ComStateMachineMaster comStateMachineMaster;
EventBroker eventBroker;

void setup()
{
  // Init Serial Monitor
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("Alive");

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // print mac address
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  ledStateMachine.init();

  button.attach(config::BUTTON_PIN, INPUT_PULLUP);
  button.interval(50);

  comStateMachineMaster.init(&ledStateMachine, &eventBroker);
  screenStateMachine.init(&eventBroker);

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  pinMode(config::BUZZER_PIN, OUTPUT);
}

void loop()
{

  if (button.fell())
  {
    for (int i = 0; i < 100; i++)
    {
      digitalWrite(config::BUZZER_PIN, LOW);
      delayMicroseconds(125);
      digitalWrite(config::BUZZER_PIN, HIGH);
      delayMicroseconds(125);
    }
    Event event;
    event.eventType = EventType::SHORT;
    eventBroker.addEvent(event);
  }

  ledStateMachine.update();
  screenStateMachine.update();
  comStateMachineMaster.update();
  button.update();
}
