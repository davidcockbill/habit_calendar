#ifndef _BUTTON_HPP_
#define _BUTTON_HPP_

#include <stdint.h>
#include <Arduino.h>

#define BUTTON_STATES C(OFF)C(ON)C(PUSH)C(LONG_PUSH)

#define C(x) x,
enum ButtonState { BUTTON_STATES };
#undef C

class Button
{
private:
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
  void changeState(ButtonState buttonState);
};

#endif