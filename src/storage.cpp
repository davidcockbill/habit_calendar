#include "storage.hpp"
#include "led_matrix.hpp"
#include <EEPROM.h>

Storage::Storage()
{
}

void Storage::saveMatrixToMemory(LedMatrix matrix)
{
    for (uint8_t month = 0; month < 12; month++)
    {
        EEPROM.put(month, matrix.getDays(month));
    }
}

void Storage::loadMatrixFromMemory(LedMatrix &matrix)
{
    for (uint8_t month = 0; month < 12; ++month)
    {
        uint32_t days = EEPROM.read(month);
        matrix.setDays(month, days);
    }
}