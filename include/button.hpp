#ifndef _BUTTON_HPP_
#define _BUTTON_HPP_

#include <stdint.h>
#include <Arduino.h>

class Button
{
private:
  static const uint8_t DEBOUNCE_TRIGGER = 5;
  String mName;
  uint8_t mPin;
  uint8_t mDebounce;
  bool mState;

public:
  Button(String name, uint8_t pin);

  void setup();
  void update();
  bool getState();
};

#endif