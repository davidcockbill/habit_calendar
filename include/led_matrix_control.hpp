#ifndef LED_MATRIX_CONTROL_HPP_
#define LED_MATRIX_CONTROL_HPP_

#include "led_matrix.hpp"
#include <stdint.h>


class LedMatrixControl
{
public:
	void configure();
	void begin();

	void toggle(uint8_t month, uint8_t day);
	LedMatrix &getMatrix();
	
	void setBrightness(uint8_t brightness);
};

#endif
