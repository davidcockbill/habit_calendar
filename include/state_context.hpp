#ifndef _STATE_CONTEXT_HPP_
#define _STATE_CONTEXT_HPP_

#include <stdint.h>
#include <display.hpp>

#include "led_matrix_control.hpp"
#include "storage.hpp"
#include "button.hpp"

#define STATES C(IDLE)C(TOGGLE)C(SELECT)C(RESET)C(DATE)C(RAM)
#define C(x) x,
enum State { STATES };
#undef C
#define C(x) #x,    
static const char *const STATE_NAME[] = { STATES };
#undef C


class StateContext
{
private:
    LedMatrixControl mLedMatrixControl;
    Storage mStorage;
    Display mDisplay;

    State mState;
    uint8_t mCurrentMonth;
    uint8_t mCurrentDay;

    uint32_t mLastMillis;
    LedMatrix mMatrixSnapshot;

public:
    StateContext();

    void begin();
    void run(ButtonState up, ButtonState down, ButtonState toggle);

private:
    void changeState(State newState);
    void displayCurrentDate();
    void displayCurrentDateWithTitle();
    void displayUnusedRam();
    void reset();

    void idleState(ButtonState up, ButtonState down, ButtonState toggle);
    void toggleState(ButtonState up, ButtonState down, ButtonState toggle);
    void selectState(ButtonState up, ButtonState down, ButtonState toggle);
    void resetState(ButtonState up, ButtonState down, ButtonState toggle);
    void dateState(ButtonState up, ButtonState down, ButtonState toggle);
    void ramState(ButtonState up, ButtonState down, ButtonState toggle);

    void incrementCurrentMonth();
    void incrementCurrentDay();
    void decrementCurrentMonth();
    void decrementCurrentDay();
};

#endif