#include <Arduino.h>

#include "led_matrix.hpp"
#include "Button.hpp"

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

void loop()
{
  static uint8_t currentMonth = 0;
  static uint8_t currentDay = 0;

  upButton.update();
  downButton.update();
  selectButton.update();

  if (selectButton.getState())
  {
    ledMatrix.toggleLED(currentMonth, currentDay);
    ++currentDay;
    delay(500);
  }
}
