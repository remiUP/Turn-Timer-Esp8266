#include <Arduino.h>
#include <ESPNowW.h>
#include <esp8266wifi.h>
#include "LedStateMachine.h"
#include "ComStateMachineMaster.h"
#include "EventBroker.h"
#include <Bounce2.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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

  button.attach(D7, INPUT_PULLUP);
  button.interval(50);

  comStateMachineMaster.init(&ledStateMachine, &eventBroker);

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  pinMode(D5, OUTPUT);
  pinMode(D7, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  display.clearDisplay();

  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner
  display.cp437(true);                 // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for (int16_t i = 0; i < 256; i++)
  {
    if (i == '\n')
      display.write(' ');
    else
      display.write(i);
  }

  display.display();
}

void loop()
{

  if (button.fell())
  {
    for (int i = 0; i < 100; i++)
    {
      digitalWrite(D5, LOW);
      delayMicroseconds(125);
      digitalWrite(D5, HIGH);
      delayMicroseconds(125);
    }
    Event event;
    event.eventType = EventType::SHORT;
    eventBroker.addEvent(event);
  }

  ledStateMachine.update();
  comStateMachineMaster.update();
  button.update();
}
