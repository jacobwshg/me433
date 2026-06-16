
#ifndef PIN_H__
#define PIN_H__ 

#include "pico/stdlib.h"
#include <cstddef>

namespace Pin
{
    static constexpr uint
        // use I2C1
        I2C_SDA { 18 }, // physical 25
        I2C_SCL { 19 }, // 24

        PWM_A   { 0 }, 
        PWM_B   { 1 }, 

        ADC0    { 26 },
        ADC1    { 27 }  
        ;

}

#endif
