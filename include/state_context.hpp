#ifndef _STATE_CONTEXT_HPP_
#define _STATE_CONTEXT_HPP_

#include <stdint.h>
#include <display.hpp>

#define STATES C(IDLE)C(TOGGLE)C(SELECT)C(RESET)
#define C(x) x,
enum State { STATES };
#undef C
#define C(x) #x,    
static const char *const STATE_NAME[] = { STATES };
#undef C

static const uint32_t STATE_IDLE_DELAY = 30000;
static const uint32_t STATE_REVERT_DELAY = 1000;

static const uint8_t DAYS_IN_MONTH[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const uint8_t MAX_MONTH = 11;

#include "led_matrix_control.hpp"
#include "storage.hpp"
#include "button.hpp"

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
    void run(ButtonState upButtonState,
             ButtonState downButtonState,
             ButtonState toggleButtonState);

private:
    void changeState(State newState);
    void displayCurrentDate();
    void reset();

    void incrementCurrentMonth();
    void incrementCurrentDay();
    void decrementCurrentMonth();
    void decrementCurrentDay();
};

#endif