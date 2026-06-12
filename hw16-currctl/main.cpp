
#include "pin.h"
#include "i2c_util.h"

#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include <cstdio>

int main()
{
    stdio_init_all();

    adc_init();
    adc_gpio_init( Pin::ADC );
    adc_select_input( 0 );

    I2CUtil::init();

    while ( true )
    {
        std::printf( "Hello, world!\n" );
        sleep_ms( 1000 );
    }
}
