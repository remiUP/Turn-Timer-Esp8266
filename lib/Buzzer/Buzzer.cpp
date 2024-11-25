#include "Buzzer.h"

Buzzer::Buzzer(uint8_t pin) {
  this->pin = pin;
  pinMode(pin, OUTPUT);
}

void Buzzer::beep(unsigned int delay) {
  digitalWrite(pin, LOW);
  delayMicroseconds(delay);
  digitalWrite(pin, HIGH);
  delayMicroseconds(delay);
}