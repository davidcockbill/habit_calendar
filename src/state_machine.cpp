#include "state_machine.hpp"
#include "state.hpp"
#include "state_context.hpp"
#include "button.hpp"
#include "logging.hpp"

// State Names Definition
#define STATE(x) #x,    
static const char *const STATE_NAME[] = { STATE_TEMPLATE };
#undef STATE


// State Entry Table Definition
#define STATE(x) void x ##_entry(StateContext&);   
STATE_TEMPLATE
#undef STATE

#define STATE(x) x ##_entry,    
void (* ENTRY_TABLE[])(StateContext&) = { STATE_TEMPLATE };
#undef STATE


// State Run Table Definition
#define STATE(x) void x ##_run(StateContext&, ButtonState, ButtonState, ButtonState);   
STATE_TEMPLATE
#undef STATE

#define STATE(x) x ##_run,    
void (* RUN_TABLE[])(StateContext&, ButtonState, ButtonState , ButtonState) = { STATE_TEMPLATE };
#undef STATE

// State Timeout Table Definition
#define STATE(x) void x ##_timeout(StateContext&);   
STATE_TEMPLATE
#undef STATE

#define STATE(x) x ##_timeout,    
void (* TIMEOUT_TABLE[])(StateContext&) = { STATE_TEMPLATE };
#undef STATE


// Constants
static const uint32_t STATE_IDLE_DELAY = 30000;
static const uint32_t STATE_REVERT_DELAY = 1000;
static const uint32_t STATE_LONG_REVERT_DELAY = 5000;


StateMachine::StateMachine():
    mContext(*this),
    mState(State::IDLE),
    mTimer()
{
}

void StateMachine::begin()
{
    mContext.begin();
}

void StateMachine::run( ButtonState up, ButtonState down, ButtonState toggle)
{
    if (mTimer.expired())
    {
        TIMEOUT_TABLE[mState](mContext);
    }

    RUN_TABLE[mState](mContext, up, down, toggle);
}

void StateMachine::changeState(State newState)
{
    mTimer.stop();

    LOGGER.info("[State] %s -> %s",
        STATE_NAME[mState],
        STATE_NAME[newState]);
    mState = newState;
    ENTRY_TABLE[mState](mContext);
}

void StateMachine::startTimer(uint32_t duration)
{
    mTimer.start(duration);
}


// IDLE State
void IDLE_entry(StateContext &context)
{
    context.displayCurrentDate();
    context.startTimer(STATE_IDLE_DELAY);
}

void IDLE_run(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
{
    if (up == ButtonState::PUSH &&
        toggle == ButtonState::ON)
    {
        context.incrementBrightness();
        context.changeState(State::BRIGHT);
    }

    if (down == ButtonState::PUSH &&
        toggle == ButtonState::ON)
    {
        context.decrementBrightness();
        context.changeState(State::BRIGHT);
    }

    if (down == ButtonState::ON &&
        up == ButtonState::ON &&
        toggle == ButtonState::ON)
    {
        context.changeState(State::RESET);
    }

    if (toggle == ButtonState::PUSH)
    {
        context.changeState(State::TOGGLE);
    }

    if (toggle == ButtonState::LONG_PUSH)
    {
        context.changeState(State::DATE);
    }

    if (toggle != ButtonState::ON && (up == ButtonState::ON || 
        down == ButtonState::ON))
    {
        context.changeState(State::SELECT);
    }
}

void IDLE_timeout(StateContext &context)
{
    context.clearDisplay();
}


// TOGGLE State
void TOGGLE_entry(StateContext &context)
{
    context.toggle();
    context.startTimer(STATE_REVERT_DELAY);
}

void TOGGLE_run(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
{
    if (toggle == ButtonState::PUSH)
    {
        context.toggle();
        context.startTimer(STATE_REVERT_DELAY);
    }
}

void TOGGLE_timeout(StateContext &context)
{
    context.incrementCurrentDay();
    context.store();
    context.changeState(State::IDLE);
}


// SELECT State
static boolean flashState = true;
static uint32_t flashTimer = millis();
void restartFlashTimer(StateContext &context)
{
    flashState = true;
    flashTimer = millis(); 
    context.setCurrentSelection(flashState);
}

void SELECT_entry(StateContext &context)
{
    context.takeSnapshot();
    context.startTimer(STATE_REVERT_DELAY);
    restartFlashTimer(context);
}

void SELECT_run(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
{
    if (millis() > flashTimer + 100)
    {
        flashState = !flashState;  
        flashTimer = millis(); 
        context.setCurrentSelection(flashState);
    }

    if (down == ButtonState::ON &&
        up == ButtonState::ON &&
        toggle == ButtonState::ON)
    {
        context.changeState(State::RESET);
    }

    if (up != ButtonState::OFF ||
        down != ButtonState::OFF)
    {
        context.startTimer(STATE_REVERT_DELAY);
    }

    if (up == ButtonState::PUSH)
    {
        context.restoreSnapshot();
        context.incrementCurrentDay();
        restartFlashTimer(context);
    }

    if (down == ButtonState::PUSH)
    {
        context.restoreSnapshot();
        context.decrementCurrentDay();
        restartFlashTimer(context);
    }

    if (up == ButtonState::LONG_PUSH)
    {
        context.restoreSnapshot();
        context.incrementCurrentMonth();
        restartFlashTimer(context);
    }

    if (down == ButtonState::LONG_PUSH)
    {
        context.restoreSnapshot();
        context.decrementCurrentMonth();
        restartFlashTimer(context);
    }
}

void SELECT_timeout(StateContext &context)
{
    context.restoreSnapshot();
    context.changeState(State::IDLE);
}


// BRIGHT State
void BRIGHT_entry(StateContext &context)
{
    context.displayBrightnessLevel();
    context.startTimer(STATE_LONG_REVERT_DELAY);
}

void BRIGHT_run(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
{
    if (up == ButtonState::PUSH &&
        toggle == ButtonState::ON)
    {
        context.incrementBrightness();
        context.displayBrightnessLevel();
    }

    if (down == ButtonState::PUSH &&
        toggle == ButtonState::ON)
    {
        context.decrementBrightness();
        context.displayBrightnessLevel();
    }
}

void BRIGHT_timeout(StateContext &context)
{
    context.changeState(State::IDLE);
}


// RESET State
void RESET_entry(StateContext &context)
{
}

void RESET_run(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
{
    if (down == ButtonState::OFF &&
        up == ButtonState::OFF &&
        toggle == ButtonState::OFF)
    {
        context.reset();
        context.changeState(State::IDLE);
    }
}

void RESET_timeout(StateContext &context)
{
    context.changeState(State::IDLE);
}


// DATE State
void DATE_entry(StateContext &context)
{
    context.startTimer(STATE_LONG_REVERT_DELAY);
    context.displayCurrentDateWithTitle();
}

void DATE_run(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
{
    if (up == ButtonState::PUSH || down == ButtonState::PUSH)
    {
        context.changeState(State::MONTH);
    }
}

void DATE_timeout(StateContext &context)
{
    context.changeState(State::IDLE);
}


// MONTH State
void MONTH_entry(StateContext &context)
{
    context.startTimer(STATE_LONG_REVERT_DELAY);
    context.displayMonthStats();
}

void MONTH_run(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
{
    if (up == ButtonState::PUSH || down == ButtonState::PUSH)
    {
        context.changeState(State::RAM);
    }
}

void MONTH_timeout(StateContext &context)
{
    context.changeState(State::IDLE);
}


// RAM State
void RAM_entry(StateContext &context)
{
    context.startTimer(STATE_LONG_REVERT_DELAY);
    context.displayUnusedRam();
}

void RAM_run(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
{
    if (up == ButtonState::PUSH || down == ButtonState::PUSH)
    {
        context.changeState(State::RESET_MONTH);
    }
}

void RAM_timeout(StateContext &context)
{
    context.changeState(State::IDLE);
}

// RESET_MONTH State
void RESET_MONTH_entry(StateContext &context)
{
    context.startTimer(STATE_LONG_REVERT_DELAY);
    context.displayResetMonth();
}

void RESET_MONTH_run(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
{
    if (up == ButtonState::PUSH || down == ButtonState::PUSH)
    {
        context.changeState(State::DATE);
    }
        
    if (toggle == ButtonState::PUSH)
    {
        context.resetCurrentMonth();
        context.changeState(State::IDLE);
    }
}

void RESET_MONTH_timeout(StateContext &context)
{
    context.changeState(State::IDLE);
}
