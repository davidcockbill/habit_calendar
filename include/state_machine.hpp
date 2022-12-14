#ifndef _STATE_MACHINE_HPP_
#define _STATE_MACHINE_HPP_

#include "button.hpp"
#include "state_context.hpp"
#include "states.hpp"


class StateMachine
{
private:
    StateContext mContext;
    State mState;

public:
    StateMachine();
    void begin();

    void run(ButtonState up, ButtonState down, ButtonState toggle);
    void changeState(State newState);

private:
    void enter();
};

#endif