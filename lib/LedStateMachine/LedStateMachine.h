#pragma once
#include <FastLED.h>
#define NUM_LEDS 4
#define LED_DATA_PIN D4

#define ANIMATION_TIME 3000

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
	CRGB leds[NUM_LEDS];
	unsigned long timer = 0;

public:
	void init();
	void update();
	void setState(LedState);
	LedState getState();
};