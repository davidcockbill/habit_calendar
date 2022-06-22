#ifndef LED_MATRIX_HPP_
#define LED_MATRIX_HPP_

#include <stdint.h>


class LedMatrix
{
public:
	void configure();
	void begin();
	void loadLedStatesFromMemory();
	void saveLedStatesToMemory();

	void setLED(uint8_t month, uint8_t day, bool enable);
	void toggleLED(uint8_t month, uint8_t day);
	void clearAllLEDs();
	void setBrightness(uint8_t brightness);
};

#endif
