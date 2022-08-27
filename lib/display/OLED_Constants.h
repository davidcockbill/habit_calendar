#ifndef _OLED_CONSTANTS_H_
#define _OLED_CONSTANTS_H_

#include <Arduino.h>
#include <Wire.h>
#include <avr/pgmspace.h>

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

#define IIC_CMD        0X00
#define IIC_RAM        0X40

#define OLED_CS 10
#define OLED_CS_0     digitalWrite(OLED_CS, LOW)
#define OLED_CS_1     digitalWrite(OLED_CS, HIGH)

#define OLED_RST 8
#define OLED_RST_0    digitalWrite(OLED_RST, LOW)
#define OLED_RST_1    digitalWrite(OLED_RST, HIGH)

#define OLED_DC 7
#define OLED_DC_0     digitalWrite(OLED_DC, LOW)
#define OLED_DC_1     digitalWrite(OLED_DC, HIGH)

#endif
