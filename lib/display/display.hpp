#ifndef _DISPLAY_HPP_
#define _DISPLAY_HPP_

#include "OLED_Driver.h"

class Display
{
private:
    uint8_t mImageCache[IMAGE_SIZE];

public:
    void configure();

    void write(const char *fmt, ...);
    void writeWithTitle(const char *title, const char *fmt, ...);
    void clear();
};

#endif