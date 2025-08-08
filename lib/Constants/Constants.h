#pragma once

#include <Arduino.h>

namespace config {

// WiFi
constexpr uint16_t RETRY_DELAY = 500;
constexpr uint16_t ACK_TIMEOUT = 2000;

// EventBroker
constexpr uint8_t EVENTQSIZE = 5;

// ComStateMachine
constexpr uint8_t MAX_NUMBER_DEVICES = 5;

// LED
constexpr uint8_t NUM_LEDS = 4;
constexpr uint8_t LED_DATA_PIN = D4;
constexpr unsigned long ANIMATION_TIME = 3000;

// Screen
constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 32;
constexpr int8_t OLED_RESET = -1;
constexpr uint8_t SCREEN_ADDRESS = 0x3C;

// Buzzer
constexpr uint8_t BUZZER_PIN = D5;

// Button
constexpr uint8_t BUTTON_PIN = D7;

}  // namespace config

