#include "state_context.hpp"
#include "logging.hpp"

static const Logger LOGGER(Level::INFO);

static const boolean RESTORE_SNAPSHOT_ON_RESET = false;
static uint32_t MATRIX_SNAPSHOT[12] = {
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x248aa006,
    0x12a96a25,
    0x24a52929,
    0x000000a1,
    0x00000000};

StateContext::StateContext():
    mState(State::IDLE),
    mCurrentMonth(0),
    mCurrentDay(0),
    mLastMillis(millis())
{
}

void StateContext::begin()
{
    LOGGER.debug("Setting up led matrix controller");
    mLedMatrixControl.configure();
    mLedMatrixControl.begin();

    mStorage.loadMatrixFromMemory(mLedMatrixControl.getMatrix());
    mStorage.loadCurrentDayFromMemory(mCurrentMonth, mCurrentDay);

    mLedMatrixControl.getMatrix().snapshot();

    LOGGER.debug("Setting up display");
    mDisplay.configure();
}

void StateContext::run(ButtonState upButtonState,
                       ButtonState downButtonState,
                       ButtonState toggleButtonState)
{

    switch (mState)
    {
        case State::IDLE:
            if ((millis() - mLastMillis) > STATE_IDLE_DELAY)
            {
                mDisplay.clear();
                mLastMillis = millis();
            }

            if (upButtonState == ButtonState::PUSH &&
                toggleButtonState == ButtonState::ON)
            {
                mLedMatrixControl.incrementBrightness();
            }

            if (downButtonState == ButtonState::PUSH &&
                toggleButtonState == ButtonState::ON)
            {
                mLedMatrixControl.decrementBrightness();
            }

            if (downButtonState == ButtonState::ON &&
                upButtonState == ButtonState::ON &&
                toggleButtonState == ButtonState::ON)
            {
                changeState(State::RESET);
            }

            if (toggleButtonState == ButtonState::PUSH)
            {
                mLedMatrixControl.getMatrix().toggle(mCurrentMonth, mCurrentDay);
                mLastMillis = millis();
                changeState(State::TOGGLE);
            }

            if (toggleButtonState != ButtonState::ON && (upButtonState == ButtonState::ON || 
                downButtonState == ButtonState::ON))
            {
                mMatrixSnapshot = mLedMatrixControl.getMatrix();
                mLastMillis = millis();
                mLedMatrixControl.getMatrix().clear();
                mLedMatrixControl.getMatrix().set(mCurrentMonth, mCurrentDay, true);
                changeState(State::SELECT);
            }
            break;

        case State::TOGGLE:
            if (toggleButtonState == ButtonState::PUSH)
            {
                mLedMatrixControl.getMatrix().toggle(mCurrentMonth, mCurrentDay);
                displayCurrentDate();
                mLastMillis = millis();
            }

            if ((millis() - mLastMillis) > STATE_REVERT_DELAY)
            {
                incrementCurrentDay();
                LOGGER.info("Month=%d, Day=%d", mCurrentMonth, mCurrentDay);
                mStorage.saveMatrixToMemory(mLedMatrixControl.getMatrix());
                mStorage.saveCurrentDayToMemory(mCurrentMonth, mCurrentDay);
                displayCurrentDate();
                changeState(State::IDLE);
            }
            break;

        case State::SELECT:
            if (downButtonState == ButtonState::ON &&
                upButtonState == ButtonState::ON &&
                toggleButtonState == ButtonState::ON)
            {
                changeState(State::RESET);
            }

            if (upButtonState != ButtonState::OFF ||
                downButtonState != ButtonState::OFF)
            {
                mLastMillis = millis();
            }

            if (upButtonState == ButtonState::PUSH)
            {
                mLedMatrixControl.getMatrix().set(mCurrentMonth, mCurrentDay, false);
                incrementCurrentDay();
                mLedMatrixControl.getMatrix().set(mCurrentMonth, mCurrentDay, true);
                displayCurrentDate();
            }

            if (downButtonState == ButtonState::PUSH)
            {
                mLedMatrixControl.getMatrix().set(mCurrentMonth, mCurrentDay, false);
                decrementCurrentDay();
                mLedMatrixControl.getMatrix().set(mCurrentMonth, mCurrentDay, true);
                displayCurrentDate();
            }

            if (upButtonState == ButtonState::LONG_PUSH)
            {
                mLedMatrixControl.getMatrix().set(mCurrentMonth, mCurrentDay, false);
                incrementCurrentMonth();
                mLedMatrixControl.getMatrix().set(mCurrentMonth, mCurrentDay, true);
                displayCurrentDate();
           }

            if (downButtonState == ButtonState::LONG_PUSH)
            {
                mLedMatrixControl.getMatrix().set(mCurrentMonth, mCurrentDay, false);
                decrementCurrentMonth();
                mLedMatrixControl.getMatrix().set(mCurrentMonth, mCurrentDay, true);
                displayCurrentDate();
            }

            if ((millis() - mLastMillis) > STATE_REVERT_DELAY)
            {
                mLedMatrixControl.getMatrix() = mMatrixSnapshot;
                LOGGER.info("Month=%d, Day=%d", mCurrentMonth, mCurrentDay);
                mStorage.saveCurrentDayToMemory(mCurrentMonth, mCurrentDay);
                changeState(State::IDLE);
            }
            break;

        case State::RESET:
            if (downButtonState == ButtonState::OFF &&
                upButtonState == ButtonState::OFF &&
                toggleButtonState == ButtonState::OFF)
            {
                reset();
                changeState(State::IDLE);
            }
            break;

        default:
            break;
    }
}

void StateContext::changeState(State newState)
{
    LOGGER.info("[State] %s -> %s",
        STATE_NAME[mState],
        STATE_NAME[newState]);
    mState = newState;
}

void StateContext::displayCurrentDate()
{
    static const char *MONTHS[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    mDisplay.write("%s %02d", MONTHS[mCurrentMonth], mCurrentDay+1);
}

void StateContext::reset()
{
    mCurrentMonth = 0;
    mCurrentDay = 0;

    if (RESTORE_SNAPSHOT_ON_RESET)
    {
        mLedMatrixControl.getMatrix().set(MATRIX_SNAPSHOT);
    }
    else
    {
        mLedMatrixControl.getMatrix().clear();
    }
    
    mStorage.saveMatrixToMemory(mLedMatrixControl.getMatrix());
    mStorage.saveCurrentDayToMemory(mCurrentMonth, mCurrentDay);
}

void StateContext::incrementCurrentMonth()
{
    ++mCurrentMonth;
    if (mCurrentMonth > MAX_MONTH)
    {
        LOGGER.debug("Month wrap");
        mCurrentMonth = 0;
    }
}

void StateContext::incrementCurrentDay()
{
    ++mCurrentDay;
    const uint32_t maxDay = DAYS_IN_MONTH[mCurrentMonth] - 1;
    if (mCurrentDay > maxDay)
    {
        LOGGER.debug("Day wrap");
        mCurrentDay = 0;
        incrementCurrentMonth();
    }
}

void StateContext::decrementCurrentMonth()
{
    if (mCurrentMonth == 0)
    {
        LOGGER.debug("Month wrap");
        mCurrentMonth = MAX_MONTH;
    }
    else
    {
        --mCurrentMonth;
    }
}

void StateContext::decrementCurrentDay()
{
    if (mCurrentDay == 0)
    {
        LOGGER.debug("Day wrap");
        decrementCurrentMonth();
        const uint32_t maxDay = DAYS_IN_MONTH[mCurrentMonth] - 1;
        mCurrentDay = maxDay;
    }
    else
    {
        --mCurrentDay;
    }
}