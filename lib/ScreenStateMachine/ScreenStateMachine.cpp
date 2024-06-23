#include "ScreenStateMachine.h"

void ScreenStateMachine::init()
{
	if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
	{
		Serial.println(F("SSD1306 allocation failed"));
	}
	display.clearDisplay();
}

unsigned long ScreenStateMachine::getMillis()
{
	if (this->getState() == ScreenState::COUNTING)
	{
		return total + (millis() - start);
	}
	return total;
}

String ScreenStateMachine::getTime()
{
	unsigned long current = this->getMillis() / 1000;
	uint32_t hours = current / 3600;
	uint32_t minutes = (current / 60) % 60;
	uint32_t seconds = current % 60;
	return (hours < 10 ? "0" : "") + String(hours) + ":" +
		   (minutes < 10 ? "0" : "") + String(minutes) + ":" +
		   (seconds < 10 ? "0" : "") + String(seconds);
}

void ScreenStateMachine::setState(ScreenState screenState)
{
	this->screenState = screenState;
};

ScreenState ScreenStateMachine::getState()
{
	return this->screenState;
}

void ScreenStateMachine::update()
{
	switch (this->screenState)
	{
	case ScreenState::OFF:
		if (previous != ScreenState::OFF)
		{
			previous = ScreenState::OFF;
			display.clearDisplay();
			display.display();
		}
		break;

	case ScreenState::COUNTING:
		if (previous != ScreenState::COUNTING)
		{
			previous = ScreenState::COUNTING;
			start = millis();
		}
		display.clearDisplay();
		display.setTextSize(2); // Draw 2X-scale text
		display.setTextColor(SSD1306_WHITE);
		display.setCursor(18, 8);
		display.println(this->getTime());
		display.display();
		break;
	case ScreenState::PAUSED:
		if (previous != ScreenState::PAUSED)
		{
			this->total += millis() - start;
			previous = ScreenState::PAUSED;
		}
		display.clearDisplay();
		display.setTextSize(2); // Draw 2X-scale text
		display.setTextColor(SSD1306_WHITE);
		display.setCursor(18, 8);
		display.println(this->getTime());
		display.display();
		break;
	default:
		this->screenState = ScreenState::OFF;
		break;
	}
}