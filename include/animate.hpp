#ifndef _ANIMATE_HPP_
#define _ANIMATE_HPP_

#include <Arduino.h>
#include "led_matrix.hpp"

class Animate
{
public:
    Animate();

    void animate(LedMatrix &matrix);
};


#endif