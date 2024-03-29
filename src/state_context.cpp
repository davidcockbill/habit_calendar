#include "state_context.hpp"
#include "state_machine.hpp"
#include "logging.hpp"
#include "memory.hpp"


static const boolean RESTORE_SNAPSHOT_ON_RESET = false;
static uint32_t MATRIX_SNAPSHOT[12] = {
    0x5529462a,
    0x0a2a1294,
    0x52548a44,
    0x000a2912,
    0x02a12910,
    0x0004a822,
    0x00000000,
    0x248aa006,
    0x12a96a25,
    0x24a52929,
    0x252550a1,
    0x24255052};

static const char *MONTHS[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static const uint8_t DAYS_IN_MONTH[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const uint8_t MAX_MONTH = 11;

StateContext::StateContext(StateMachine &stateMachine):
    mStateMachine(stateMachine),
    mCurrentMonth(0),
    mCurrentDay(0)
{
}

void StateContext::begin()
{
    LOGGER.debug(F("Setting up led matrix controller"));
    mLedMatrixControl.configure();
    mLedMatrixControl.begin();

    mStorage.loadMatrixFromMemory(mLedMatrixControl.getMatrix());
    mStorage.loadCurrentDayFromMemory(mCurrentMonth, mCurrentDay);

    mLedMatrixControl.getMatrix().snapshot();

    LOGGER.debug(F("Setting up display"));
    mDisplay.configure();
}

void StateContext::changeState(State newState)
{
    mStateMachine.changeState(newState);
}

void StateContext::startTimer(uint32_t duration)
{
    mStateMachine.startTimer(duration);
}

void StateContext::store()
{
    mStorage.saveMatrixToMemory(mLedMatrixControl.getMatrix());
    mStorage.saveCurrentDayToMemory(mCurrentMonth, mCurrentDay);
}


void StateContext::incrementBrightness()
{
    mLedMatrixControl.incrementBrightness();
}

void StateContext::decrementBrightness()
{
    mLedMatrixControl.decrementBrightness();
}

void StateContext::toggle()
{
    mLedMatrixControl.getMatrix().toggle(mCurrentMonth, mCurrentDay);
    displayCurrentDate();
}

void StateContext::setCurrentSelection(bool enable)
{
    mLedMatrixControl.getMatrix().set(mCurrentMonth, mCurrentDay, enable);
}

void StateContext::clear()
{
    mLedMatrixControl.getMatrix().clear();
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
        clear();
    }
    
    store();
}

void StateContext::resetCurrentMonth()
{
    mLedMatrixControl.getMatrix().setDays(mCurrentMonth, 0);    
    store();
}

void StateContext::takeSnapshot()
{
    mMatrixSnapshot = mLedMatrixControl.getMatrix();
}

void StateContext::restoreSnapshot()
{
    mLedMatrixControl.getMatrix() = mMatrixSnapshot;
    LOGGER.info("Month=%d, Day=%d", mCurrentMonth, mCurrentDay);
    mStorage.saveCurrentDayToMemory(mCurrentMonth, mCurrentDay);
}

void StateContext::clearDisplay()
{
    mDisplay.clear();
}

void StateContext::displayCurrentDate()
{
    mDisplay.write("%s %02d", MONTHS[mCurrentMonth], mCurrentDay+1);
}

void StateContext::displayCurrentDateWithTitle()
{
    mDisplay.writeWithTitle("Date:", "%s %02d", MONTHS[mCurrentMonth], mCurrentDay+1);
}

void StateContext::displayBrightnessLevel()
{
    mDisplay.write("%02d", mLedMatrixControl.getBrightnessLevel());
}

void StateContext::displayUnusedRam()
{
    int unusedRam = getUnusedRam();
    mDisplay.writeWithTitle("RAM:", "%d bytes", unusedRam);
}

void StateContext::displayMonthStats()
{
    uint8_t maxDay = DAYS_IN_MONTH[mCurrentMonth];
    uint32_t bits = mLedMatrixControl.getMatrix().getDays(mCurrentMonth);
    uint8_t days = 0;
    while (bits)
    {
        days += bits & 1;
        bits >>= 1;
    }
    mDisplay.writeWithTitle(MONTHS[mCurrentMonth], "%d/%d", days, maxDay);
}

void StateContext::displayResetMonth()
{
    mDisplay.writeWithTitle("Reset:", "%s?", MONTHS[mCurrentMonth]);
}

void StateContext::displayReset()
{
    mDisplay.writeWithTitle("Reset:", "All?");
}

void StateContext::incrementCurrentMonth()
{
    ++mCurrentMonth;
    if (mCurrentMonth > MAX_MONTH)
    {
        LOGGER.debug(F("Month wrap"));
        mCurrentMonth = 0;
    }
    displayCurrentDate();
}

void StateContext::incrementCurrentDay()
{
    ++mCurrentDay;
    const uint32_t maxDay = DAYS_IN_MONTH[mCurrentMonth] - 1;
    if (mCurrentDay > maxDay)
    {
        LOGGER.debug(F("Day wrap"));
        mCurrentDay = 0;
        incrementCurrentMonth();
    }
    displayCurrentDate();
}

void StateContext::decrementCurrentMonth()
{
    if (mCurrentMonth == 0)
    {
        LOGGER.debug(F("Month wrap"));
        mCurrentMonth = MAX_MONTH;
    }
    else
    {
        --mCurrentMonth;
    }
    displayCurrentDate();
}

void StateContext::decrementCurrentDay()
{
    if (mCurrentDay == 0)
    {
        LOGGER.debug(F("Day wrap"));
        decrementCurrentMonth();
        const uint32_t maxDay = DAYS_IN_MONTH[mCurrentMonth] - 1;
        mCurrentDay = maxDay;
    }
    else
    {
        --mCurrentDay;
    }
    displayCurrentDate();
}