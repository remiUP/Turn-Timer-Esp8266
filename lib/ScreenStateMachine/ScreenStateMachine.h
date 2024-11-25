#pragma once
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "EventBroker.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET -1		// Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

enum class ScreenState
{
	OFF,
	COUNTING,
	PAUSED
};

class ScreenStateMachine
{
private:
	Adafruit_SSD1306 display;
	EventBroker *eventBroker;
	ScreenState screenState = ScreenState::COUNTING;
	unsigned long total = 0;
	unsigned long start = 0;
	unsigned long getMillis();
	String getTime();
	void printTime();
	ScreenState previous = ScreenState::OFF;

public:
	ScreenStateMachine() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET){};
	void init(EventBroker *);
	void update();
	void setState(ScreenState);
	ScreenState getState();
};