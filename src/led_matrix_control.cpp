#include "led_matrix_control.hpp"
#include "led_matrix.hpp"
#include "logging.hpp"

#include <Arduino.h>
#include <SPI.h>

static const int SRCLK_PIN = 10;
static const int OE_PIN = 9;
static const uint8_t MAX_BRIGHTNESS = 254;
static const uint8_t BRIGHTNESS_VALUES[] = {0, 10, 30, 60, 100, 128, 180, 254};
static const uint8_t MAX_BRIGHTNESS_IDX = (sizeof(BRIGHTNESS_VALUES) / sizeof(BRIGHTNESS_VALUES[0])) - 1;

static uint8_t brightness_idx = MAX_BRIGHTNESS_IDX/2;
static LedMatrix matrix;

void LedMatrixControl::configure()
{
    // LED configuration
    SPI.begin();
    pinMode (SRCLK_PIN, OUTPUT);
    digitalWrite (SRCLK_PIN, HIGH);
    pinMode (OE_PIN, OUTPUT);
    digitalWrite(OE_PIN, HIGH);

    // Timer2 interrupt configuration
    TCCR2A = (TCCR2A & ~0x03) | 0x00;
    TCCR2B = (TCCR2B & ~0x08) | 0x00;
    TCCR2B = (TCCR2B & ~0x07) | 0x04; // clock prescaler of 64
    OCR2A = ~(BRIGHTNESS_VALUES[brightness_idx]);

    matrix.clear();
}

void LedMatrixControl::begin()
{
    TIMSK2 = (1<<OCIE2A) | (1<<TOIE2); // Enable Timers
}

LedMatrix &LedMatrixControl::getMatrix()
{
    return matrix;
}

void logBrightness()
{
    LOGGER.info("Brightness[%d]: %d", brightness_idx, BRIGHTNESS_VALUES[brightness_idx]);
}

void LedMatrixControl::incrementBrightness()
{
    if (brightness_idx < MAX_BRIGHTNESS_IDX)
    {
        ++brightness_idx;
    }
    logBrightness();

    // Turn on brightness interrupt
    if (brightness_idx > 0)
    {
        TIMSK2 |= (1<<OCIE2A);
    }
}

void LedMatrixControl::decrementBrightness()
{
    if (brightness_idx > 0)
    {
        --brightness_idx;
    }
    logBrightness();

    // Turn off brightness interrupt
    if (brightness_idx == 0)
    {
        TIMSK2 &= ~(1<<OCIE2A);
    }
}

ISR(TIMER2_COMPA_vect)
{
    digitalWrite(OE_PIN, LOW); // Enable
}

ISR(TIMER2_OVF_vect)
{
    static const SPISettings spiSettings(4000000, MSBFIRST, SPI_MODE0);
    static uint8_t currentMonth = 0;
    static uint8_t spiBuf[6];

    digitalWrite(OE_PIN, HIGH); // Disable

    OCR2A = ~(BRIGHTNESS_VALUES[brightness_idx]);

    uint16_t month = (1 << currentMonth);
    uint32_t days = matrix.getDays(currentMonth);

    // Send the LED control values into the shift registers
    digitalWrite (SRCLK_PIN, LOW);
    SPI.beginTransaction(spiSettings);

    spiBuf[0] = (uint8_t)(month >> 8 & 0x00FF);
    spiBuf[1] = (uint8_t)(month >> 0 & 0x00FF);
    spiBuf[2] = (uint8_t)(days >> 24 & 0x000000FF);
    spiBuf[3] = (uint8_t)(days >> 16 & 0x000000FF);
    spiBuf[4] = (uint8_t)(days >> 8 & 0x000000FF);
    spiBuf[5] = (uint8_t)(days >> 0 & 0x000000FF);

    SPI.transfer(spiBuf, sizeof(spiBuf));
    SPI.endTransaction();
    digitalWrite (SRCLK_PIN, HIGH);

    currentMonth++;
    currentMonth %= 12;
}
