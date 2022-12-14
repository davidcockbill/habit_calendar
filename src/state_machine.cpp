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


// Constants
static const uint32_t STATE_IDLE_DELAY = 30000;
static const uint32_t STATE_REVERT_DELAY = 1000;
static const uint32_t STATE_LONG_REVERT_DELAY = 5000;


StateMachine::StateMachine():
    mContext(*this),
    mState(State::IDLE)
{
}

void StateMachine::begin()
{
    mContext.begin();
}

void StateMachine::enter()
{
    ENTRY_TABLE[mState](mContext);
}

void StateMachine::run( ButtonState up, ButtonState down, ButtonState toggle)
{
    RUN_TABLE[mState](mContext, up, down, toggle);
}

void StateMachine::changeState(State newState)
{
    LOGGER.info("[State] %s -> %s",
        STATE_NAME[mState],
        STATE_NAME[newState]);
    mState = newState;
    enter();
}

// IDLE State
void IDLE_entry(StateContext &context)
{
    context.displayCurrentDate();
    context.restartTimer();
}

void IDLE_run(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
{
    if (context.timerExpired(STATE_IDLE_DELAY))
    {
        context.clearDisplay();
        context.restartTimer();
    }

    if (up == ButtonState::PUSH &&
        toggle == ButtonState::ON)
    {
        context.incrementBrightness();
    }

    if (down == ButtonState::PUSH &&
        toggle == ButtonState::ON)
    {
        context.decrementBrightness();
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

// TOGGLE State
void TOGGLE_entry(StateContext &context)
{
    context.toggle();
    context.restartTimer();
}

void TOGGLE_run(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
{
    if (toggle == ButtonState::PUSH)
    {
        context.toggle();
        context.displayCurrentDate();
        context.restartTimer();
    }

    if (context.timerExpired(STATE_REVERT_DELAY))
    {
        context.incrementCurrentDay();
        context.store();
        context.displayCurrentDate();
        context.changeState(State::IDLE);
    }
}

// SELECT State
void SELECT_entry(StateContext &context)
{
    context.takeSnapshot();
    context.restartTimer();
    context.clear();
    context.setCurrentSelection(true);
}

void SELECT_run(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
{
    if (down == ButtonState::ON &&
        up == ButtonState::ON &&
        toggle == ButtonState::ON)
    {
        context.changeState(State::RESET);
    }

    if (up != ButtonState::OFF ||
        down != ButtonState::OFF)
    {
        context.restartTimer();
    }

    if (up == ButtonState::PUSH)
    {
        context.setCurrentSelection(false);
        context.incrementCurrentDay();
        context.setCurrentSelection(true);
        context.displayCurrentDate();
    }

    if (down == ButtonState::PUSH)
    {
        context.setCurrentSelection(false);
        context.decrementCurrentDay();
        context.setCurrentSelection(true);
        context.displayCurrentDate();
    }

    if (up == ButtonState::LONG_PUSH)
    {
        context.setCurrentSelection(false);
        context.incrementCurrentMonth();
        context.setCurrentSelection(true);
        context.displayCurrentDate();
    }

    if (down == ButtonState::LONG_PUSH)
    {
        context.setCurrentSelection(false);
        context.decrementCurrentMonth();
        context.setCurrentSelection(true);
        context.displayCurrentDate();
    }

    if (context.timerExpired(STATE_REVERT_DELAY))
    {
        context.restoreSnapshot();
        context.changeState(State::IDLE);
    }
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

// DATE State
void DATE_entry(StateContext &context)
{
    context.restartTimer();
    context.displayCurrentDateWithTitle();
}

void DATE_run(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
{
    if (up == ButtonState::PUSH || down == ButtonState::PUSH)
    {
        context.changeState(State::MONTH);
    }

    if (context.timerExpired(STATE_LONG_REVERT_DELAY))
    {
        context.changeState(State::IDLE);
    }
}

// MONTH State
void MONTH_entry(StateContext &context)
{
    context.restartTimer();
    context.displayMonthStats();
}

void MONTH_run(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
{
    if (up == ButtonState::PUSH || down == ButtonState::PUSH)
    {
        context.changeState(State::RAM);
    }

    if (context.timerExpired(STATE_LONG_REVERT_DELAY))
    {
        context.changeState(State::IDLE);
    }
}

// RAM State
void RAM_entry(StateContext &context)
{
    context.restartTimer();
    context.displayUnusedRam();
}

void RAM_run(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
{
    if (up == ButtonState::PUSH || down == ButtonState::PUSH)
    {
        context.changeState(State::DATE);
    }

    if (context.timerExpired(STATE_LONG_REVERT_DELAY))
    {
        context.changeState(State::IDLE);
    }
}
