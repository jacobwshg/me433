
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
        PWM1    { 20 }, // 26
        PWM2    { 21 },
        // use ADC0
        ADC     { 26 } // 31 
        ;

}

#endif
