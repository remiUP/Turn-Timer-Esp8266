#include <Arduino.h>

class Buzzer {
 public:
  Buzzer(uint8_t pin);
  void beep(unsigned int);

 private:
  uint8_t pin;
}