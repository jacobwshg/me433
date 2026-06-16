
#ifndef PIN_H__
#define PIN_H__

#include "pico/stdlib.h"

namespace Pin
{

    // INA219 current sensor 
    static constexpr uint
        I2C_SDA { 16 },
        I2C_SCL { 17 };
    
    static constexpr uint
        PWM_A { 20 },
        PWM_B { 21 };

    static constexpr uint
        ADC0 { 26 },
        ADC1 { 27 };
}

#endif
