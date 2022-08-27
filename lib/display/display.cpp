#include "display.hpp"
#include "OLED_Driver.h"
#include "GUI_Paint.h"
#include <fonts.h>

void Display::configure()
{
    OLED_Init();
    delay(500);
    OLED_Clear();

    Paint_NewImage(mImageCache, OLED_HEIGHT, OLED_WIDTH, 90, BLACK);
    Paint_SelectImage(mImageCache);
    Paint_Clear(BLACK);
    delay(500);
}

void Display::write(const char *fmt, ...)
{    
    va_list args;
    va_start(args, fmt);
    char buffer[100];

    if ((uint32_t)vsnprintf(buffer, sizeof(buffer), fmt, args) <= sizeof(buffer))
    {
        Paint_Clear(BLACK);
        Paint_DrawString_EN(8, 8, buffer, &Font24, WHITE, WHITE);
        OLED_Display(mImageCache);
    }
    va_end(args);
}

void Display::clear()
{    
    Paint_Clear(BLACK);
    OLED_Display(mImageCache);
}