
#include "pin.h"
#include "util.h"
#include "pwm_util.h"
#include "i2c_util.h"
#include "HX711.h"
#include "INA219.h"

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include <cstdio>


#define I2C_SDA 8
#define I2C_SCL 9


int main()
{
    stdio_init_all();

    I2CUtil::init();
    PWMUtil::init();

    HX711::init();
    INA219::init();

    while ( true )
    {
        sleep_ms( 10 );
    }
}
