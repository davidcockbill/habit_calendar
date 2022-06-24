#include "Button.hpp"
#include "logging.hpp"
#include <Arduino.h>

#define C(x) #x,    
static const char *const BUTTON_STATE_NAME[] = { BUTTON_STATES };
#undef C

static const uint32_t DEBOUNCE_DELAY = 5;
static const uint32_t LONG_PUSH_DELAY = 250;
static const Logger LOGGER(Level::DEBUG);

Button::Button(String name, uint8_t pin): 
    mName(name),
    mPin(pin),
    mLastPinValue(LOW),
    mLastDebounceTime(0),
    mLastPushTime(0),
    mButtonState(ButtonState::OFF)
{
}

void Button::setup()
{
    pinMode(mPin, INPUT);
}

ButtonState Button::getState()
{
    uint8_t currentPinValue = digitalRead(mPin);
    if (currentPinValue != mLastPinValue)
    {
        mLastDebounceTime = millis();
    }

    if ((millis() - mLastDebounceTime) > DEBOUNCE_DELAY)
    {
        if (mButtonState == ButtonState::PUSH || mButtonState == ButtonState::LONG_PUSH)
        {
            changeState(ButtonState::OFF);
        }

        if (mButtonState == ButtonState::OFF && currentPinValue == HIGH)
        {
            mLastPushTime = millis();
            changeState(ButtonState::ON);
        }

        if (mButtonState == ButtonState::ON && currentPinValue == LOW)
        {
            if ((millis() - mLastPushTime) > LONG_PUSH_DELAY)
            {
                changeState(ButtonState::LONG_PUSH);
            }
            else
            {
                changeState(ButtonState::PUSH);
            }
        }
    }
    mLastPinValue = currentPinValue;

    return mButtonState;
}

void Button::changeState(ButtonState buttonState)
{
    LOGGER.debug("[%s Button] %s -> %s",
        mName.c_str(),
        BUTTON_STATE_NAME[mButtonState],
        BUTTON_STATE_NAME[buttonState]);
    mButtonState = buttonState;
}
