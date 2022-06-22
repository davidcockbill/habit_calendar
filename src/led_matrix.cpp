#include "led_matrix.hpp"
#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>

#define UINT16_MSB(uint16) ((uint8_t)(uint16 >> 8))
#define UINT16_LSB(uint16) ((uint8_t)(uint16 & 0xFF))
#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))
#define EEPROM_START_ADR  0x00000000

static const int csPin = 10;
static const int outputEnablePin = 8;
// set up the speed, data order and data mode
static SPISettings spiSettings(4000000, MSBFIRST, SPI_MODE0);
static const uint8_t maxBrightness = 255;
static uint8_t brightness = maxBrightness/2;
static uint32_t ledValues[12] = {0}; // Months are the integers in range [0,11], Days are bits within the integers, in range [0,31]


void LedMatrix::configure()
{
  // LED configurations
  SPI.begin();
  pinMode (csPin, OUTPUT);
  digitalWrite (csPin, HIGH);
  pinMode (outputEnablePin, OUTPUT);
  digitalWrite(outputEnablePin, HIGH);
  // Enable Timer2 interrupt.
  // We want CTC mode (mode 2) where timer resets after compare
  TCCR2A = (TCCR2A & ~0x03) | 0x00;
  TCCR2B = (TCCR2B & ~0x08) | 0x00;
  // TCCR2B = (TCCR2B & ~0x07) | 0x02; // selects a clock prescaler of 8. That's a frequency of 31372.55
  TCCR2B = (TCCR2B & ~0x07) | 0x04; // selects a clock prescaler of 8. That's a frequency of 31372.55
  OCR2A = brightness; //128
  clearAllLEDs();
}

void LedMatrix::begin()
{
  TIMSK2 = (1<<OCIE2A) | (1<<TOIE2); // Enable Timers
}

void LedMatrix::clearAllLEDs()
{
  memset(ledValues, 0, sizeof(ledValues));
}

// Month is in range [0,11]
// Day is in range [0,30]
void LedMatrix::setLED(uint8_t month, uint8_t day, bool enable)
{
  if (month > 11 || day > 30){
    return;
  }

  if (enable)
  {
      ledValues[month] = ledValues[month] | ((uint32_t)1 << day);
  }
  else
  {
      ledValues[month] = ledValues[month] & ~((uint32_t)1 << day);
  }
}

void LedMatrix::toggleLED(uint8_t month, uint8_t day)
{
   bool ledState = (*(ledValues+month) & ((uint32_t)1 << (day)));
   setLED(month, day, !ledState);
}

void LedMatrix::saveLedStatesToMemory()
{
  for(uint8_t month=0; month<12; month++)
  {
    int addr = EEPROM_START_ADR + (month * sizeof(uint32_t));
    EEPROM.put(addr, ledValues[month]);
  }
}

void LedMatrix::loadLedStatesFromMemory(){
  // If the first month is completely set, then this is the first time we're running the code.
  // Clear the calendar memory
  uint32_t firstInt;
  EEPROM.get(EEPROM_START_ADR, firstInt);
  if(firstInt == 0xFFFFFFFF)
  {
    Serial.println("First time running everyday calendar.  Clearing all days.");
    for(uint32_t i=0; i<sizeof(ledValues); i++)
    {
      EEPROM.write(EEPROM_START_ADR + i, 0x00);
    }
  }
  // Load calendar from memory
  for(int i=0; i<(int)sizeof(ledValues); i++)
  {
    *((byte*)ledValues + i) = EEPROM.read(EEPROM_START_ADR + i);
  }

  for(int i=0; i<12; i++)
  {
    Serial.print("LED Column ");
    Serial.print(i);
    Serial.print(" = ");
    Serial.println(ledValues[i]);
  }
}

void LedMatrix::setBrightness(uint8_t b)
{
  if (b > 200)
  {
    b = 200;
  }
  b = ~b;
  if ((brightness == 255) && (b != 255))
  {
    TIMSK2 |= (1<<OCIE2A);
  }
  brightness = b;
  if (brightness == 255)
  {
    TIMSK2 &= ~(1<<OCIE2A);
  }
}

// Code to drive the LED multiplexing.
// This code is called at a very fast period, activating each LED column one by one
// This is done faster than the eye can perceive, and it appears that all twelve columns are illuminated at once.
ISR(TIMER2_COMPA_vect)
{
  digitalWrite(outputEnablePin, LOW); // Enable
}

ISR(TIMER2_OVF_vect)
{
    static uint8_t activeColumn = 0;
    // static byte spiBuf[6];
    static uint8_t spiBuf[2];

    digitalWrite(outputEnablePin, HIGH); // Disable

    OCR2A = brightness;

    uint16_t month = (1 << activeColumn);
    uint8_t * pDays = (uint8_t *)(ledValues + activeColumn);

    // Send the LED control values into the shift registers
    digitalWrite (csPin, LOW);
    SPI.beginTransaction(spiSettings);

    // memcpy(spiBuf, &month, 2);
    // spiBuf[2] = pDays[3];
    // spiBuf[3] = pDays[2];
    // spiBuf[4] = pDays[1];
    // spiBuf[5] = pDays[0];

    spiBuf[0] = month;
    spiBuf[1] = pDays[0];

    SPI.transfer(spiBuf, sizeof(spiBuf));
    SPI.endTransaction();
    digitalWrite (csPin, HIGH);

    activeColumn++;
    activeColumn %= 12;
}
