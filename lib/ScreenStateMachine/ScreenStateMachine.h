#pragma once

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "EventBroker.h"
#include "Constants.h"

enum class ScreenState {
        OFF,
        COUNTING,
        PAUSED
};

class ScreenStateMachine {
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
        ScreenStateMachine()
            : display(config::SCREEN_WIDTH, config::SCREEN_HEIGHT, &Wire,
                      config::OLED_RESET){};
        void init(EventBroker *);
        void update();
        void setState(ScreenState);
        ScreenState getState();
};

