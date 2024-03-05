#include "LedStateMachine.h"

double nSin(const unsigned long x, const unsigned int period, const double phase)
{
	return (1 + sin((x % period) * 2 * PI / period + phase * 2 * PI)) / 2;
}

double nExp(const double x, const double coef)
{
	return (exp(x * coef) - 1) / (exp(coef) - 1);
}

void smoothRotation(const CRGB &color, const unsigned int period, CRGB (&leds)[NUM_LEDS])
{
	for (int i = 0; i < NUM_LEDS; i++)
	{
		double intensity = nExp(nSin(millis(), period, (double)i / NUM_LEDS), 1);
		leds[i].red = color.red * intensity;
		leds[i].green = color.green * intensity;
		leds[i].blue = color.blue * intensity;
	}
	FastLED.show();
}

void breathe(const CRGB &color, const unsigned int period, CRGB (&leds)[NUM_LEDS], uint8_t depth)
{
	double intensity = nExp(nSin(millis(), period, 0), 1) * (double)depth / 255 + 1 - (double)depth / 255;
	fill_solid(leds, NUM_LEDS, CRGB(color.r * intensity, color.g * intensity, color.b * intensity));
	FastLED.show();
}

void LedStateMachine::init()
{
	FastLED.addLeds<WS2812, LED_DATA_PIN, GRB>(leds, NUM_LEDS); // GRB ordering is typical
	FastLED.setBrightness(50);
}
LedState previous = LedState::IDLE;

void LedStateMachine::setState(LedState ledState)
{
	this->ledState = ledState;
};

LedState LedStateMachine::getState()
{
	return this->ledState;
}

void LedStateMachine::update()
{
	switch (this->ledState)
	{
	case LedState::IDLE:
		break;

	case LedState::OFF:
		if (previous != LedState::OFF)
		{
			previous = LedState::OFF;
			fill_solid(leds, NUM_LEDS, CRGB::Black);
			FastLED.show();
			Serial.println("LED STATE : OFF");
		}
		break;

	case LedState::ON:
		if (previous != LedState::ON)
		{
			previous = LedState::ON;
			Serial.println("LED STATE : ON");
		}
		breathe(CRGB::White, 4000, leds, 200);
		break;

	case LedState::PAIRING:
		if (previous != LedState::PAIRING)
		{
			previous = LedState::PAIRING;
			Serial.println("LED STATE : PAIRING");
		}
		smoothRotation(CRGB::Blue, 2000, leds);
		break;

	case LedState::PAIRED:
		if (previous != LedState::PAIRING)
		{
			previous = LedState::PAIRING;
			Serial.println("LED STATE : PAIRED");
		}
		breathe(CRGB::Green, 4000, leds, 200);
		break;

	case LedState::REORDER:
	{
		if (previous != LedState::REORDER)
		{
			previous = LedState::REORDER;
			Serial.println("LED STATE : REORDER");
		}
		smoothRotation(CRGB::Yellow, 2000, leds);
		break;
	}
	case LedState::ERROR:
		if (previous != LedState::ERROR)
		{
			previous = LedState::ERROR;
			fill_solid(leds, NUM_LEDS, CRGB::Red);
			FastLED.show();
			Serial.println("LED STATE : ERROR");
			this->ledState = LedState::IDLE;
		}
		break;

	default:
		this->ledState = LedState::IDLE;
		break;
	}
}