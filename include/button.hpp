#ifndef _BUTTON_HPP_
#define _BUTTON_HPP_

#include <stdint.h>
#include <Arduino.h>

enum ButtonState { OFF, ON, PUSH, LONG_PUSH };


class Button
{
private:
  static const uint32_t DEBOUNCE_DELAY = 50;
  static const uint32_t LONG_PUSH_DELAY = 1000;

  String mName;
  uint8_t mPin;
  uint8_t mLastPinValue;
  uint32_t mLastDebounceTime;
  uint32_t mLastPushTime;
  ButtonState mButtonState;

public:
  Button(String name, uint8_t pin);

  void setup();
  ButtonState getState();

private:
 void dumpState();
};

#endif