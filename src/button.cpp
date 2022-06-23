#include "Button.hpp"
#include <Arduino.h>



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
            mButtonState = ButtonState::OFF;
        }

        if (mButtonState == ButtonState::OFF && currentPinValue == HIGH)
        {
            mLastPushTime = millis();
            mButtonState = ButtonState::ON;
        }

        if (mButtonState == ButtonState::ON && currentPinValue == LOW)
        {
            if ((millis() - mLastPushTime) > LONG_PUSH_DELAY)
            {
                mButtonState = ButtonState::LONG_PUSH;
            }
            else
            {
                mButtonState = ButtonState::PUSH;
            }
        }
        
    }
    mLastPinValue = currentPinValue;

    return mButtonState;
}

void Button::dumpState()
{
    Serial.print("[");
    Serial.print(mName);
    Serial.print("] ");
    Serial.println((mButtonState == HIGH) ? "On" : "Off");
}
