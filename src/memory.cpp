#include "memory.hpp"
#include <Arduino.h>

#define MEMORY_MARKER 0x55


uint8_t *currentHeapPtr()
{
    extern int __heap_start, *__brkval;
    int *heap_ptr = __brkval == 0 ? &__heap_start : __brkval;
    return (uint8_t *)heap_ptr;
}

int freeRam()
{
  int value;
  int *currentStackPtr = &value;
  return (int)currentStackPtr - (int)currentHeapPtr;  
}

void initialiseFreeRam()
{
  uint8_t *heap_ptr = currentHeapPtr();
  uint8_t value;
  while (heap_ptr < &value)
  {
    *heap_ptr = MEMORY_MARKER;
    ++heap_ptr;
  }
}

int getUnusedRam()
{
  uint8_t *heap_ptr = currentHeapPtr();
  int value = 0;

  while ((int)heap_ptr < (int)&value)
  {
    if (*heap_ptr == MEMORY_MARKER)
    {
        ++value;
    }
    else
    {
        if (value > 2)
        {
            break;
        }
        value = 0;
    }
    ++heap_ptr;
  }

  return value;
}
