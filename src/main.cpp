#include <Arduino.h>

#include "led_matrix_control.hpp"
#include "button.hpp"
#include "logging.hpp"
#include "storage.hpp"
#include "animate.hpp"


#define STATES C(IDLE)C(TOGGLE)C(SELECT)C(RESET)
#define C(x) x,
enum State { STATES };
#undef C
#define C(x) #x,    
static const char *const STATE_NAME[] = { STATES };
#undef C

// static const uint8_t DAYS_IN_MONTH[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const uint8_t DAYS_IN_MONTH[] = {8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};
static const uint8_t MAX_MONTH = 11;

static const uint32_t STATE_REVERT_DELAY = 1000;
static const Logger LOGGER(Level::INFO);
static LedMatrixControl ledMatrixControl;
static uint8_t currentMonth = 0;
static uint8_t currentDay = 0;
static Storage storage;
static Button upButton("Up", PIND3);
static Button downButton("Down", PIND2);
static Button toggleButton("Toggle", PIND4);

void setup()
{
    Serial.begin(9600);
    LOGGER.info("Setup: start");

    LOGGER.debug("Setting up buttons");
    upButton.setup();
    downButton.setup();
    toggleButton.setup();

    LOGGER.debug("Setting up led matrix controller");
    ledMatrixControl.configure();
    ledMatrixControl.begin();
    storage.loadMatrixFromMemory(ledMatrixControl.getMatrix());
    storage.loadCurrentDayFromMemory(currentMonth, currentDay);

    LOGGER.info("Setup: complete");
}

void incrementCurrentMonth(uint8_t &month)
{
    ++month;
    if (month > MAX_MONTH)
    {
        LOGGER.debug("Month wrap");
        month = 0;
    }
}

void incrementCurrentDay(uint8_t &month, uint8_t &day)
{
    ++day;
    const uint32_t maxDay = DAYS_IN_MONTH[month] - 1;
    if (day > maxDay)
    {
        LOGGER.debug("Day wrap");
        day = 0;
        incrementCurrentMonth(month);
    }
}

void decrementCurrentMonth(uint8_t &month)
{
    if (month == 0)
    {
        LOGGER.debug("Month wrap");
        month = MAX_MONTH;
    }
    else
    {
        --month;
    }
}

void decrementCurrentDay(uint8_t &month, uint8_t &day)
{
    if (day == 0)
    {
        LOGGER.debug("Day wrap");
        decrementCurrentMonth(month);
        const uint32_t maxDay = DAYS_IN_MONTH[month] - 1;
        day = maxDay;
    }
    else
    {
        --day;
    }
}

void changeState(State &state, State newState)
{
    LOGGER.info("[State] %s -> %s",
        STATE_NAME[state],
        STATE_NAME[newState]);
    state = newState;
}

void loop()
{
    static State state = State::IDLE;
    static uint32_t lastMillis = millis();
    static LedMatrix matrixSnapshot;

    uint32_t start = micros();
    ButtonState upButtonState = upButton.getState();
    ButtonState downButtonState = downButton.getState();
    ButtonState toggleButtonState = toggleButton.getState();

    switch (state)
    {
        case State::IDLE:
            if (downButtonState == ButtonState::ON &&
                upButtonState == ButtonState::ON &&
                toggleButtonState == ButtonState::ON)
            {
                changeState(state, State::RESET);
            }

            if (toggleButtonState == ButtonState::PUSH)
            {
                ledMatrixControl.getMatrix().toggle(currentMonth, currentDay);
                lastMillis = millis();
                changeState(state, State::TOGGLE);
            }

            if (upButtonState == ButtonState::ON || 
                downButtonState == ButtonState::ON)
            {
                matrixSnapshot = ledMatrixControl.getMatrix();
                lastMillis = millis();
                ledMatrixControl.getMatrix().clear();
                ledMatrixControl.getMatrix().set(currentMonth, currentDay, true);
                changeState(state, State::SELECT);
            }
            break;

        case State::TOGGLE:
            if (toggleButtonState == ButtonState::PUSH)
            {
                ledMatrixControl.getMatrix().toggle(currentMonth, currentDay);
                lastMillis = millis();
            }

            if ((millis() - lastMillis) > STATE_REVERT_DELAY)
            {
                incrementCurrentDay(currentMonth, currentDay);
                LOGGER.info("Month=%d, Day=%d", currentMonth, currentDay);
                storage.saveMatrixToMemory(ledMatrixControl.getMatrix());
                storage.saveCurrentDayToMemory(currentMonth, currentDay);
                changeState(state, State::IDLE);
            }
            break;

        case State::SELECT:
            if (downButtonState == ButtonState::ON &&
                upButtonState == ButtonState::ON &&
                toggleButtonState == ButtonState::ON)
            {
                changeState(state, State::RESET);
            }

            if (upButtonState != ButtonState::OFF ||
                downButtonState != ButtonState::OFF)
            {
                lastMillis = millis();
            }

            if (upButtonState == ButtonState::PUSH)
            {
                ledMatrixControl.getMatrix().set(currentMonth, currentDay, false);
                incrementCurrentDay(currentMonth, currentDay);
                ledMatrixControl.getMatrix().set(currentMonth, currentDay, true);
            }

            if (downButtonState == ButtonState::PUSH)
            {
                ledMatrixControl.getMatrix().set(currentMonth, currentDay, false);
                decrementCurrentDay(currentMonth, currentDay);
                ledMatrixControl.getMatrix().set(currentMonth, currentDay, true);
            }

            if (upButtonState == ButtonState::LONG_PUSH)
            {
                ledMatrixControl.getMatrix().set(currentMonth, currentDay, false);
                incrementCurrentMonth(currentMonth);
                ledMatrixControl.getMatrix().set(currentMonth, currentDay, true);
            }

            if (downButtonState == ButtonState::LONG_PUSH)
            {
                ledMatrixControl.getMatrix().set(currentMonth, currentDay, false);
                decrementCurrentMonth(currentMonth);
                ledMatrixControl.getMatrix().set(currentMonth, currentDay, true);
            }

            if ((millis() - lastMillis) > STATE_REVERT_DELAY)
            {
                ledMatrixControl.getMatrix() = matrixSnapshot;
                LOGGER.info("Month=%d, Day=%d", currentMonth, currentDay);
                storage.saveCurrentDayToMemory(currentMonth, currentDay);
                changeState(state, State::IDLE);
            }
            break;

        case State::RESET:
            if (downButtonState == ButtonState::OFF &&
                upButtonState == ButtonState::OFF &&
                toggleButtonState == ButtonState::OFF)
            {
                Animate().animate(ledMatrixControl.getMatrix());
                currentMonth = 0;
                currentDay = 0;
                ledMatrixControl.getMatrix().clear();
                storage.saveMatrixToMemory(ledMatrixControl.getMatrix());
                storage.saveCurrentDayToMemory(currentMonth, currentDay);
                changeState(state, State::IDLE);
            }
            break;

        default:
            break;
    }

    uint32_t duration = micros() - start;
    LOGGER.trace("Loop completed in %dus", duration);
}
