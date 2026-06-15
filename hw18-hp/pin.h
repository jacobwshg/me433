
#ifndef PIN_H__
#define PIN_H__

#include "pico/stdlib.h"

namespace Pin
{
    // HX711 force sensor ( custom prot. )
    static constexpr uint
        FSENS_DT  { 15 },
        FSENS_SCK { 16 };

    // INA219 current sensor 
    static constexpr uint
        I2C_SDA { 18 },
        I2C_SCL { 19 };
    
    static constexpr uint
        PWM_A { 20 },
        PWM_B { 21 };

    static constexpr uint
        ADC0 { 26 },
        ADC1 { 27 };
}

#endif
