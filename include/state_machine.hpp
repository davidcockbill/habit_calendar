#ifndef _STATE_MACHINE_HPP_
#define _STATE_MACHINE_HPP_

#include "button.hpp"
#include "state_context.hpp"
#include "state.hpp"
#include "state_timer.hpp"


class StateMachine
{
private:
    StateContext mContext;
    State mState;
    StateTimer mTimer;

public:
    StateMachine();
    void begin();

    void run(ButtonState up, ButtonState down, ButtonState toggle);
    void changeState(State newState);
    void startTimer(uint32_t duration);

private:
    void stopTimer();
    bool timerExpired();
};

#endif