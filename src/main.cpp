#include <Arduino.h>

#include "led_matrix.hpp"
#include "Button.hpp"
#include "logging.hpp"


#define STATES C(IDLE)C(TOGGLE)C(RESET)
#define C(x) x,
enum State { STATES };
#undef C
#define C(x) #x,    
static const char *const STATE_NAME[] = { STATES };
#undef C

static const Logger LOGGER(Level::INFO);
static LedMatrix ledMatrix;
static Button upButton("Up", PIND2);
static Button downButton("Down", PIND3);
static Button selectButton("Select", PIND4);

void setup()
{
  Serial.begin(9600);
  LOGGER.info("Setup: start");

  LOGGER.debug("Settting up buttons");
  upButton.setup();
  downButton.setup();
  selectButton.setup();

  LOGGER.debug("Settting up led matrix");
  ledMatrix.configure();
  ledMatrix.begin();

  LOGGER.info("Setup: complete");
}

void incrementCurrentDay(uint8_t &month, uint8_t &day)
{
  static const uint8_t MAX_DAY = 7;
  static const uint8_t MAX_MONTH = 1;

  ++day;
  if (day > MAX_DAY)
  {
    LOGGER.debug("Day wrap");
    day = 0;
    ++month;
    if (month > MAX_MONTH)
    {
      LOGGER.debug("Month wrap");
      month = 0;
    }
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
  static uint8_t currentMonth = 0;
  static uint8_t currentDay = 0;
  static uint32_t lastMillis = millis();

  uint32_t start = micros();
  ButtonState upButtonState = upButton.getState();
  ButtonState downButtonState = downButton.getState();
  ButtonState selectButtonState = selectButton.getState();

  switch (state)
  {
    case State::IDLE:
      if (downButtonState == ButtonState::ON &&
          upButtonState == ButtonState::ON &&
          selectButtonState == ButtonState::ON)
      {
        changeState(state, State::RESET);
      }

      if (selectButtonState == ButtonState::PUSH)
      {
        ledMatrix.toggleLED(currentMonth, currentDay);
        lastMillis = millis();
        changeState(state, State::TOGGLE);
      }
      break;

    case State::TOGGLE:
      if (selectButtonState == ButtonState::PUSH)
      {
        ledMatrix.toggleLED(currentMonth, currentDay);
        lastMillis = millis();
      }

      if ((millis() - lastMillis) > 1000)
      {
        incrementCurrentDay(currentMonth, currentDay);
        changeState(state, State::IDLE);
      }
      break;

    case State::RESET:
      if (downButtonState == ButtonState::OFF &&
          upButtonState == ButtonState::OFF &&
          selectButtonState == ButtonState::OFF)
      {
        currentMonth = 0;
        currentDay = 0;
        ledMatrix.clearAllLEDs();
        changeState(state, State::IDLE);
      }
      break;

    default:
      break;
  }

  uint32_t duration = micros() - start;
  LOGGER.trace("Loop completed in %dus", duration);
}
