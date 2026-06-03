
//#include <stdio.h>
#include "font.h"
#include "ssd1306.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include <cstdio>
#include <cstdint>
#include <array>

// I2C defines
// This example will use I2C0 on GPIO4 (SDA) and GPIO5 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0

namespace Pins
{
    static constexpr uint
        I2C_SDA_PIN { 20 },
        I2C_SCL_PIN { 21 },
        BLINK_PIN   { 0 },
        ADC_PIN     { 26 },
        ADC_IN_PIN  { 0 }
    ;
}

int main()
{
    bool blink_on { true };
    absolute_time_t
        blink_then { 0 }, blink_now { 0 },
        fps_then { 0 }, fps_now { 0 };

    std::array< char, 128 > msg_V {}, msg_fps {};

    stdio_init_all();

    gpio_init( Pins::BLINK_PIN );
    gpio_set_dir( Pins::BLINK_PIN, GPIO_OUT );
    gpio_put( Pins::BLINK_PIN, blink_on );

    // I2C Initialisation.
    i2c_init( I2C_PORT, 400 * 1000 );

    gpio_set_function( Pins::I2C_SDA_PIN, GPIO_FUNC_I2C );
    gpio_set_function( Pins::I2C_SCL_PIN, GPIO_FUNC_I2C );
    gpio_pull_up( Pins::I2C_SDA_PIN );
    gpio_pull_up( Pins::I2C_SCL_PIN );
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    adc_init();
    adc_gpio_init( Pins::ADC_PIN );
    adc_select_input( Pins::ADC_IN_PIN );

    SSD1306::init(); 

    // SSD1306::draw_msg(
    //     "When I left you, I was but the learner; now I am the Master.",
    //     0, 0
    // );
    // SSD1306::update();

    while ( true )
    {
        //std::printf( "Hello, world!\n" );

        const std::uint16_t adc_value = adc_read();
        const float V = ( ( float ) adc_value * 3.3f ) / ( float )( 1 << 12 );

        blink_now = get_absolute_time();
        if ( blink_now - blink_then >= 5e5 ) // 500 ms
        {
            blink_on = !blink_on;
            gpio_put( Pins::BLINK_PIN, blink_on );
            blink_then = blink_now;
        }
        fps_now = get_absolute_time();
        const absolute_time_t fps_dt = fps_now - fps_then;
        const float fps = 1e6f / ( float ) fps_dt;
        fps_then = fps_now;

        std::snprintf( msg_V.data(), msg_V.size(), "ADC voltage @ GPIO%u: %.2f V ( 0x%04x )", Pins::ADC_IN_PIN, V, adc_value );
        std::snprintf( msg_fps.data(), msg_fps.size(), "FPS: %.2f", fps );

        SSD1306::clear();
        SSD1306::draw_msg( msg_V.data(), 0, 0 );
        SSD1306::draw_msg( msg_fps.data(), 0, 24 );
        SSD1306::update();

        msg_V.fill( 0 ); msg_fps.fill( 0 );

    }
}
