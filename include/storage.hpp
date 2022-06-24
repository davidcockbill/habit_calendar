#ifndef _STORAGE_HPP_
#define _STORAGE_HPP_

#include "led_matrix.hpp"

class Storage
{
private:

public:
    Storage();

	void saveMatrixToMemory(LedMatrix matrix);
	void loadMatrixFromMemory(LedMatrix &matrix);
};

#endif