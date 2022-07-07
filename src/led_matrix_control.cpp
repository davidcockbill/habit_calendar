#include "led_matrix_control.hpp"
#include "led_matrix.hpp"

#include <Arduino.h>
#include <SPI.h>

static const int SRCLK_PIN = 10;
static const int OE_PIN = 9;
static const uint8_t MAX_BRIGHTNESS = 255;

static uint8_t brightness = MAX_BRIGHTNESS/2;
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
    OCR2A = brightness;

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

void LedMatrixControl::setBrightness(uint8_t b)
{
    if (b > 200)
    {
        b = 200;
    }

    b = ~b;
    if ((brightness == 255) && (b != 255))
    {
        TIMSK2 |= (1<<OCIE2A);
    }
    brightness = b;
    if (brightness == 255)
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

    OCR2A = brightness;

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
