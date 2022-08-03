#include "led_matrix.hpp"
#include "logging.hpp"
#include <Arduino.h>

static const Logger LOGGER(Level::INFO);


LedMatrix::LedMatrix(): mMatrix{0}
{
    clear();
}

void LedMatrix::all()
{
    memset(mMatrix, 0xFF, sizeof(mMatrix));
}

void LedMatrix::clear()
{
    memset(mMatrix, 0, sizeof(mMatrix));
}

void LedMatrix::set(uint32_t values[12])
{
    memcpy(mMatrix, values, sizeof(mMatrix));
}

void LedMatrix::set(uint8_t month, uint8_t day, bool enable)
{
    if (month <= 11 && day <= 31)
    {
        if (enable)
        {
            mMatrix[month] = mMatrix[month] | ((uint32_t)1 << day);
        }
        else
        {
            mMatrix[month] = mMatrix[month] & ~((uint32_t)1 << day);
        }
    }
}

void LedMatrix::toggle(uint8_t month, uint8_t day)
{
    bool ledState = mMatrix[month] & ((uint32_t)1 << (day));
    set(month, day, !ledState);
}

uint32_t LedMatrix::getDays(uint8_t month)
{
    return mMatrix[month];
}

void LedMatrix::setDays(uint8_t month, uint32_t days)
{
    mMatrix[month] = days;
}

void LedMatrix::snapshot()
{
    LOGGER.info("static uint32_t MATRIX_SNAPSHOT[12] = {");
    for (uint8_t month = 0; month < 12; ++month)
    {
        const char *endToken = (month == 11) ? "};" : ",";
        char line[20] = "    0x%08lx";
        LOGGER.info(strcat(line, endToken), mMatrix[month]);
    }
}