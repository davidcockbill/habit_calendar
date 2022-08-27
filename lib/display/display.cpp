#include "display.hpp"
#include "OLED_Constants.h"
#include "OLED_Driver.h"
#include "GUI_Paint.h"
#include <fonts.h>

static UBYTE ImageCache[IMAGE_SIZE];

int initialiseDisplay()
{
    OLED_Init();
    delay(500);
    OLED_Clear();

    Paint_NewImage(ImageCache, OLED_HEIGHT, OLED_WIDTH, 90, BLACK);
    Paint_SelectImage(ImageCache);
    Paint_Clear(BLACK);
    delay(500);
    return 0;
}

void displayWrite(const char *fmt, ...)
{    
    va_list args;
    va_start(args, fmt);
    char buffer[100];

    if ((uint32_t)vsnprintf(buffer, sizeof(buffer), fmt, args) <= sizeof(buffer))
    {
        Paint_Clear(BLACK);
        Paint_DrawString_EN(8, 8, buffer, &Font24, WHITE, WHITE);
        OLED_Display(ImageCache);
    }
    va_end(args);
}

void displayClear()
{    
    Paint_Clear(BLACK);
    OLED_Display(ImageCache);
}