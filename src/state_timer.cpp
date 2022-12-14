#include <Arduino.h>
#include "state_timer.hpp"

StateTimer::StateTimer():
    mTimeout(0),
    mTimerRunning(false)
{
}

void StateTimer::start(uint32_t duration)
{
    mTimeout = millis() + duration;
    mTimerRunning = true;
}

void StateTimer::stop()
{
    mTimerRunning = false;
}

boolean StateTimer::expired()
{
    if (mTimerRunning && mTimeout < millis())
    {
        mTimerRunning = false;
        return true;
    }
    return false;
}
