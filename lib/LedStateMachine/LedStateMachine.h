#pragma once
#include <FastLED.h>
#include "Constants.h"

enum class LedState
{
	IDLE,
	OFF,
	ON,
	PAIRING,
	PAIRED,
	REORDER,
	ERROR
};

class LedStateMachine
{
private:
	LedState ledState = LedState::OFF;
        CRGB leds[config::NUM_LEDS];
	unsigned long timer = 0;

public:
	void init();
	void update();
	void setState(LedState);
	LedState getState();
};