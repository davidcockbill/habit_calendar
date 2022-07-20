#ifndef _LED_MATRIX_HPP_
#define _LED_MATRIX_HPP_

#include <stdint.h>

class LedMatrix
{
private:
    uint32_t mMatrix[12];

public:
    LedMatrix();

    void all();
    void clear();

    void set(uint8_t month, uint8_t day, bool enable);
    void toggle(uint8_t month, uint8_t day);

    uint32_t getDays(uint8_t month);
    void setDays(uint8_t month, uint32_t days);
};

#endif