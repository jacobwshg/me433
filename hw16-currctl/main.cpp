
#include "pin.h"
#include "i2c_util.h"
#include "pwm_util.h"

#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include <cstdio>
#include <cstdint>

int main()
{
    stdio_init_all();

    adc_init();
    adc_gpio_init( Pin::ADC );
    adc_select_input( 0 );

    I2CUtil::init();

    while ( true )
    {
        const std::uint16_t adc_val { adc_read() };

        std::printf( "%u\n", adc_val );
        sleep_ms( 100 );
    }
}
