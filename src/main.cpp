#include <Arduino.h>

#include "state_context.hpp"
#include "button.hpp"
#include "logging.hpp"

static const Logger LOGGER(Level::INFO);

static StateContext stateContext;
static Button upButton("Up", PIND3);
static Button downButton("Down", PIND2);
static Button toggleButton("Toggle", PIND4);

void setup()
{
    Serial.begin(115200);
    LOGGER.info("Setup: start");

    LOGGER.debug("Setting up buttons");
    upButton.setup();
    downButton.setup();
    toggleButton.setup();

    LOGGER.debug("Setting State Machine");
    stateContext.begin();

    LOGGER.info("Setup: complete");
}

void loop()
{
    uint32_t start = micros();
    ButtonState upButtonState = upButton.getState();
    ButtonState downButtonState = downButton.getState();
    ButtonState toggleButtonState = toggleButton.getState();

    stateContext.run(upButtonState, downButtonState, toggleButtonState);

    uint32_t duration = micros() - start;
    LOGGER.trace("Loop completed in %dus", duration);
}
