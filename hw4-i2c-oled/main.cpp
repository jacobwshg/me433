
//#include <stdio.h>
#include "font.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <cstdio>
#include <cstdint>
#include <array>

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
//#define I2C_SDA 8
//#define I2C_SCL 9

namespace Pins
{
    static constexpr uint
        I2C_SDA_PIN { 4 },
        I2C_SCL_PIN { 5 },
        BLINK_PIN   { 17 }
    ;
}

using ascii_bm_t = std::array< std::uint8_t, 5 >;

int main()
{
    stdio_init_all();

    gpio_init( Pins::BLINK_PIN );
    gpio_set_dir( Pins::BLINK_PIN, GPIO_OUT );
    static bool led_state { false };

    // I2C Initialisation. Using it at 400Khz.
    i2c_init( I2C_PORT, 400*1000 );

    gpio_set_function( Pins::I2C_SDA_PIN, GPIO_FUNC_I2C );
    gpio_set_function( Pins::I2C_SCL_PIN, GPIO_FUNC_I2C );
    gpio_pull_up( Pins::I2C_SDA_PIN );
    gpio_pull_up( Pins::I2C_SCL_PIN );
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    while ( true )
    {
        gpio_put( Pins::BLINK_PIN, led_state );
        led_state = !led_state;
        //std::printf( "Hello, world!\n" );
        sleep_ms( 1000 );
    }
}
