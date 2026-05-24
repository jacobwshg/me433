
#include "mpu6050.h"
#include "ssd1306.h"
#include "i2c_def.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <cstdio>
#include <cstring>
#include <limits>
#include <array>

// I2C defines
// This example will use I2C0 on GPIO4 (SDA) and GPIO5 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_SDA 4
#define I2C_SCL 5

#define BLINK_PIN 0

int main()
{
    using SSD1306::px_pos_t;

    bool blink_on { true };

    stdio_init_all();

    gpio_init( BLINK_PIN );
    gpio_set_dir( BLINK_PIN, GPIO_OUT );

    // I2C Initialisation. Using it at 400Khz.
    i2c_init( I2C_PORT, 400*1000 );

    gpio_set_function( I2C_OLED_SDA_PIN, GPIO_FUNC_I2C ); gpio_pull_up( I2C_OLED_SDA_PIN );
    gpio_set_function( I2C_OLED_SCL_PIN, GPIO_FUNC_I2C ); gpio_pull_up( I2C_OLED_SCL_PIN );
    gpio_set_function( I2C_IMU_SDA_PIN, GPIO_FUNC_I2C );  gpio_pull_up( I2C_IMU_SDA_PIN );
    gpio_set_function( I2C_IMU_SCL_PIN, GPIO_FUNC_I2C );  gpio_pull_up( I2C_IMU_SCL_PIN );

    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    SSD1306::init();
    SSD1306::draw_crosshair();
    SSD1306::update();

    MPU6050::init();

    MPU6050::SensorData sensordata {};

    std::array< char, 64 > msgbuf {};

    while ( true )
    {
        msgbuf.fill( 0x0 );
        // gpio_put( BLINK_PIN, blink_on );
        // blink_on = !blink_on;
        // sleep_ms( 500 );
        SSD1306::clear();
        SSD1306::draw_crosshair();

        MPU6050::read_sensor( sensordata );

        const std::uint8_t whoami { MPU6050::read_whoami() };
        snprintf( msgbuf.data(), msgbuf.size(), "whoami: 0x%02x", whoami );
        SSD1306::draw_msg( msgbuf.data(), 0, 0 );
        msgbuf.fill( 0x0 );

        snprintf( msgbuf.data(), msgbuf.size(), "accel_x: 0x%04x", sensordata.accel_x );
        SSD1306::draw_msg( msgbuf.data(), 0, 8 );
        msgbuf.fill( 0x0 );
        snprintf( msgbuf.data(), msgbuf.size(), "accel_y: 0x%04x", sensordata.accel_y );
        SSD1306::draw_msg( msgbuf.data(), 0, 16 );
        msgbuf.fill( 0x0 );
        snprintf( msgbuf.data(), msgbuf.size(), "accel_z: 0x%04x", sensordata.accel_z );
        SSD1306::draw_msg( msgbuf.data(), 0, 24 );
        msgbuf.fill( 0x0 );

        static constexpr px_pos_t MAX_OFS { static_cast< px_pos_t >( std::min( SSD1306::WIDTH/2, SSD1306::HEIGHT/2 ) ) };
        static constexpr px_pos_t I16_MAX { std::numeric_limits< px_pos_t >::max() };
        static constexpr float F_I16_MAX { static_cast< float >( I16_MAX ) };

        const float x_scale { static_cast< float >( sensordata.accel_x ) / F_I16_MAX }; 
        const float y_scale { static_cast< float >( sensordata.accel_y ) / F_I16_MAX }; 

        const px_pos_t x_ofs { static_cast< px_pos_t >( x_scale * MAX_OFS ) };
        const px_pos_t y_ofs { static_cast< px_pos_t >( y_scale * MAX_OFS ) };

        const px_pos_t
            x { SSD1306::XC + x_ofs },
            y { SSD1306::YC + y_ofs }; 
        SSD1306::drawpixel( x, y, true );
        SSD1306::update();

    }
}
