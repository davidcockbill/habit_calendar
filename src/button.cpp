#include "Button.hpp"
#include <Arduino.h>

Button::Button(String name, uint8_t pin): 
    mName(name),
    mPin(pin),
    mDebounce(0),
    mState(false)
{
}

void Button::setup()
{
    pinMode(mPin, INPUT);
}

void Button::update()
{
    boolean up = digitalRead(mPin);
    if (mState)
    {
        if (not up)
        {
            ++mDebounce;
            if (mDebounce >= DEBOUNCE_TRIGGER)
            {
                Serial.print("[");
                Serial.print(mName.c_str());
                Serial.println("] OFF");
                mState = false;
                mDebounce = 0;
            }
        }
        else
        {
           mDebounce = 0;
        }
    }
    else
    {
         if (up)
        {
            ++mDebounce;
            if (mDebounce >= DEBOUNCE_TRIGGER)
            {
                Serial.print("[");
                Serial.print(mName.c_str());
                Serial.println("] ON");
                mState = true;
                mDebounce = 0;
            }
        }
        else
        {
           mDebounce = 0;
        }
    }
}

bool Button::getState()
{
    return mState;
}
