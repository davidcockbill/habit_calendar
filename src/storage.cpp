#include "storage.hpp"
#include "led_matrix.hpp"
#include "logging.hpp"
#include <EEPROM.h>

static const int CURRENT_MONTH_IDX = 48;
static const int CURRENT_DAY_IDX = 49;


Storage::Storage()
{
}

void Storage::saveMatrixToMemory(LedMatrix matrix)
{
    LOGGER.info("Saving matrix");

    for (uint8_t month = 0; month < 12; month++)
    {
        int idx = month * sizeof(uint32_t);
        EEPROM.put(idx, matrix.getDays(month));
    }
}

void Storage::loadMatrixFromMemory(LedMatrix &matrix)
{
    LOGGER.info(F("Loading matrix"));

    for (uint8_t month = 0; month < 12; ++month)
    {
        uint32_t days = 0;
        for (uint8_t byte = 0; byte < sizeof(uint32_t); ++byte)
        {
            int idx = (month * sizeof(uint32_t)) + byte;
            uint8_t value = EEPROM.read(idx);
            days |= (uint32_t)value << (byte * 8);
        }

        LOGGER.debug("month=%02d, days=0x%08lx", month, days);
        matrix.setDays(month, days);
    }
}

void Storage::saveCurrentDayToMemory(uint8_t currentMonth, uint8_t currrentDay)
{
    EEPROM.put(CURRENT_MONTH_IDX, currentMonth);
    EEPROM.put(CURRENT_DAY_IDX, currrentDay);
}

void Storage::loadCurrentDayFromMemory(uint8_t &currentMonth, uint8_t &currrentDay)
{
    currentMonth = EEPROM.read(CURRENT_MONTH_IDX);
    currrentDay = EEPROM.read(CURRENT_DAY_IDX);
}