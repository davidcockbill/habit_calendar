#include "state_machine.hpp"
#include "states.hpp"
#include "state_context.hpp"
#include "button.hpp"
#include "logging.hpp"


#define C(x) #x,    
static const char *const STATE_NAME[] = { STATES };
#undef C

#define C(x) void x ##_entry(StateContext&);   
STATES
#undef C

#define C(x) x ##_entry,    
void (* ENTRY_TABLE[])(StateContext&) = { STATES };
#undef C


#define C(x) void x ##_state(StateContext&, ButtonState, ButtonState, ButtonState);   
STATES
#undef C

#define C(x) x ##_state,    
void (* RUN_TABLE[])(StateContext&, ButtonState, ButtonState , ButtonState) = { STATES };
#undef C

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

// IDLE STATE
void IDLE_entry(StateContext &context)
{
}

void IDLE_state(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
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
        context.toggle();
        context.restartTimer();
        context.changeState(State::TOGGLE);
    }

    if (toggle == ButtonState::LONG_PUSH)
    {
        context.restartTimer();
        context.displayCurrentDateWithTitle();
        context.changeState(State::DATE);
    }

    if (toggle != ButtonState::ON && (up == ButtonState::ON || 
        down == ButtonState::ON))
    {
        context.takeSnapshot();
        context.restartTimer();
        context.clear();
        context.setCurrentSelection(true);
        context.changeState(State::SELECT);
    }
}

// TOGGLE STATE
void TOGGLE_entry(StateContext &context)
{
}

void TOGGLE_state(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
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

// SELECT STATE
void SELECT_entry(StateContext &context)
{
}

void SELECT_state(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
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

// RESET STATE
void RESET_entry(StateContext &context)
{
}

void RESET_state(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
{
    if (down == ButtonState::OFF &&
        up == ButtonState::OFF &&
        toggle == ButtonState::OFF)
    {
        context.reset();
        context.changeState(State::IDLE);
    }
}

// DATE STATE
void DATE_entry(StateContext &context)
{
}

void DATE_state(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
{
    if (up == ButtonState::PUSH || down == ButtonState::PUSH)
    {
        context.restartTimer();
        context.displayUnusedRam();
        context.changeState(State::RAM);
    }

    if (context.timerExpired(STATE_LONG_REVERT_DELAY))
    {
        context.displayCurrentDate();
        context.changeState(State::IDLE);
    }
}

// RAM STATE
void RAM_entry(StateContext &context)
{
}

void RAM_state(StateContext &context, ButtonState up, ButtonState down, ButtonState toggle)
{
    if (up == ButtonState::PUSH || down == ButtonState::PUSH)
    {
        context.restartTimer();
        context.displayCurrentDateWithTitle();
        context.changeState(State::DATE);
    }

    if (context.timerExpired(STATE_LONG_REVERT_DELAY))
    {
        context.displayCurrentDate();
        context.changeState(State::IDLE);
    }
}
