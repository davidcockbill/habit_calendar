#include <Arduino.h>

#include "state_machine.hpp"
#include "button.hpp"
#include "logging.hpp"
#include "memory.hpp"

static StateMachine stateMachine;
static Button upButton("Up", PIND3);
static Button downButton("Down", PIND2);
static Button toggleButton("Toggle", PIND4);

void setup()
{
    initialiseFreeRam();

    Serial.begin(115200);
    LOGGER.info(F("Setup: start"));

    LOGGER.debug(F("Setting up buttons"));
    upButton.setup();
    downButton.setup();
    toggleButton.setup();

    LOGGER.debug(F("Setting up State Machine"));
    stateMachine.begin();

    LOGGER.info(F("Setup: complete"));
}

void loop()
{
    uint32_t start = micros();

    ButtonState up = upButton.getState();
    ButtonState down = downButton.getState();
    ButtonState toggle = toggleButton.getState();
    stateMachine.run(up, down, toggle);

    uint32_t duration = micros() - start;
    LOGGER.trace("Loop completed in %dus", duration);
}
