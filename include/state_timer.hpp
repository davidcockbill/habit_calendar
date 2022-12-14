#ifndef _STATE_TIMER_HPP_
#define _STATE_TIMER_HPP_

#include <stdint.h>


class StateTimer
{
private:
    uint32_t mTimeout;
    bool mTimerRunning;

public:
    StateTimer();
    
    void start(uint32_t duration);
    void stop();
    boolean expired();
};


#endif