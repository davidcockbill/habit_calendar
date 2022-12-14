#ifndef _STATE_CONTEXT_HPP_
#define _STATE_CONTEXT_HPP_

#include <stdint.h>
#include <display.hpp>

#include "led_matrix_control.hpp"
#include "storage.hpp"
#include "button.hpp"
#include "state.hpp"

class StateMachine;

class StateContext
{
private:
    StateMachine &mStateMachine;
    LedMatrixControl mLedMatrixControl;
    Storage mStorage;
    Display mDisplay;

    uint8_t mCurrentMonth;
    uint8_t mCurrentDay;

    uint32_t mLastMillis;
    LedMatrix mMatrixSnapshot;

public:
    StateContext(StateMachine &stateMachine);

    void begin();

    // State
    void changeState(State newState);
    void restartTimer();
    boolean timerExpired(uint32_t expiryTime);

    // Storage
    void store();

    // Matrix
	void incrementBrightness();
	void decrementBrightness();
    void toggle();
    void setCurrentSelection(bool enable);
    void clear();
    void takeSnapshot();
    void restoreSnapshot();
    void reset();

    // Display
    void clearDisplay();
    void displayCurrentDate();
    void displayCurrentDateWithTitle();
    void displayUnusedRam();
    void displayMonthStats();

    // Current Selection
    void incrementCurrentMonth();
    void incrementCurrentDay();
    void decrementCurrentMonth();
    void decrementCurrentDay();
};

#endif