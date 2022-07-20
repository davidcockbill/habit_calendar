#include "led_matrix.hpp"
#include <Arduino.h>


LedMatrix::LedMatrix(): mMatrix{0}
{
    clear();
}

void LedMatrix::all()
{
    memset(mMatrix, 0xFFFFFFFF, sizeof(mMatrix));
}

void LedMatrix::clear()
{
    memset(mMatrix, 0, sizeof(mMatrix));
}

void LedMatrix::set(uint8_t month, uint8_t day, bool enable)
{
    if (month <= 11 && day <= 30)
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
