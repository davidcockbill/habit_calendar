#ifndef LED_MATRIX_CONTROL_HPP_
#define LED_MATRIX_CONTROL_HPP_

#include "led_matrix.hpp"
#include <stdint.h>


class LedMatrixControl
{
public:
	void configure();
	void begin();

	LedMatrix &getMatrix();
	
	void incrementBrightness();
	void decrementBrightness();
	uint8_t getBrightnessLevel();
};

#endif
