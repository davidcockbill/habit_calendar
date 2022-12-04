#ifndef _MEMORY_HPP_
#define _MEMORY_HPP_

#include <Arduino.h>


int freeRam()
{
  extern int __heap_start,*__brkval;
  int v;
  return (int)&v - (__brkval == 0  
    ? (int)&__heap_start : (int) __brkval);  
}

void display_freeram()
{
  Serial.print(F("SRAM free space: "));
  Serial.println(freeRam());
}



#endif
