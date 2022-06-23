#include <Arduino.h>

#include "led_matrix.hpp"
#include "Button.hpp"

 enum State { IDLE, TOGGLE, RESET };

static LedMatrix ledMatrix;
static Button upButton("UP", PIND2);
static Button downButton("DOWN", PIND3);
static Button selectButton("SELECT", PIND4);

void setup()
{
  Serial.begin(9600);
  Serial.println("Setup: start");

  Serial.println("Setup: buttons");
  upButton.setup();
  downButton.setup();
  selectButton.setup();

  Serial.println("Setup: led matrix");
  ledMatrix.configure();
  ledMatrix.begin();

  Serial.println("Setup: done");
}

void incrementCurrentDay(uint8_t &month, uint8_t &day)
{
  static const uint8_t MAX_DAY = 7;
  static const uint8_t MAX_MONTH = 1;

  ++day;
  if (day > MAX_DAY)
  {
    Serial.println("Day wrap");
    day = 0;
    ++month;
    if (month > MAX_MONTH)
    {
      Serial.println("Month wrap");
      month = 0;
    }
  }
}

void loop()
{
  static State state = State::IDLE;
  static uint8_t currentMonth = 0;
  static uint8_t currentDay = 0;
  static uint32_t lastMillis = millis();

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
        state = State::RESET;
      }

      if (selectButtonState == ButtonState::PUSH)
      {
        ledMatrix.toggleLED(currentMonth, currentDay);
        lastMillis = millis();
        state = State::TOGGLE;
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
        state = State::IDLE;
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
        state = State::IDLE;
      }
      break;

    default:
      break;
  }


}
