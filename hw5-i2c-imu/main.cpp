
#include "mpu6050.h"
#include "ssd1306.h"
#include "i2c_util.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <cstdio>
#include <cstring>
#include <limits>
#include <array>

#define BLINK_PIN 0

int main()
{
    using SSD1306::px_pos_t;

    int blink_on { 1 };

    stdio_init_all();

    gpio_init( BLINK_PIN );
    gpio_set_dir( BLINK_PIN, GPIO_OUT );
    gpio_put( BLINK_PIN, blink_on );

    // I2C Initialisation. Using it at 100Khz.
    i2c_init( I2C_PORT, 100*1000 );

    gpio_set_function( I2C_SDA_PIN, GPIO_FUNC_I2C ); gpio_pull_up( I2C_SDA_PIN );
    gpio_set_function( I2C_SCL_PIN, GPIO_FUNC_I2C ); gpio_pull_up( I2C_SCL_PIN );

    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    SSD1306::init();
    SSD1306::draw_crosshair();
    SSD1306::update();

    MPU6050::init();
    const std::uint8_t whoami { MPU6050::read_whoami() };

    static std::array< char, 64 > msgbuf {};

    while ( true )
    {
        msgbuf.fill( 0x0 );
        gpio_put( BLINK_PIN, blink_on );
        blink_on = !blink_on;
        // sleep_ms( 500 );
        SSD1306::clear();
        SSD1306::draw_crosshair();

        MPU6050::SensorData sensordata { MPU6050::read_sensor() };

                
        const std::int16_t
            temp { sensordata.temp },
            accel_x { sensordata.accel_x },
            accel_y { sensordata.accel_y },
            accel_z { sensordata.accel_z },
            gyro_x { sensordata.gyro_x },
            gyro_y { sensordata.gyro_y },
            gyro_z { sensordata.gyro_z }
            ;
        //std::printf( "temp: %6d  accel: %6d %6d %6d  gyro: %6d %6d %6d\n", temp, accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z );
        std::printf( "%6d\n", accel_x );

        std::int16_t
            accel_0 { -2 * accel_x },
            accel_1 { 2 * accel_y };

        //
        // print WHO_AM_I
        //
        snprintf( msgbuf.data(), msgbuf.size(), "whoami:" );
        SSD1306::draw_msg( msgbuf.data(), 0, 0 );
        msgbuf.fill( 0x0 );

        snprintf( msgbuf.data(), msgbuf.size(), "0x%02x", whoami );
        SSD1306::draw_msg( msgbuf.data(), 0, 8 );
        msgbuf.fill( 0x0 );

        //
        // display temperature
        //
        snprintf( msgbuf.data(), msgbuf.size(), "temp:" );
        SSD1306::draw_msg( msgbuf.data(), 0, 16 );
        msgbuf.fill( 0x0 );

        const float temp_f { static_cast< float >( temp ) / 340.0F + 36.53F };
        snprintf( msgbuf.data(), msgbuf.size(), "%05.2f C", temp_f );
        SSD1306::draw_msg( msgbuf.data(), 0, 24 );
        msgbuf.fill( 0x0 );


        //
        // draw acceleration vector
        //
        static constexpr px_pos_t MAX_OFS { static_cast< px_pos_t >( std::min( SSD1306::WIDTH/2, SSD1306::HEIGHT/2 ) ) };
        static constexpr px_pos_t I16_MAX { std::numeric_limits< px_pos_t >::max() };
        static constexpr float F_I16_MAX { static_cast< float >( I16_MAX ) };
        static constexpr float SCALE { static_cast< float >( MAX_OFS ) / F_I16_MAX };

        const px_pos_t x_ofs { static_cast< px_pos_t >( accel_0 * SCALE ) };
        const px_pos_t y_ofs { static_cast< px_pos_t >( accel_1 * SCALE ) };

        const px_pos_t
            x { SSD1306::XC + x_ofs },
            y { SSD1306::YC + y_ofs }; 
        SSD1306::drawpixel( x, y, true );
        SSD1306::draw_segment( SSD1306::XC, SSD1306::YC, x, y );
        SSD1306::update();

        gpio_put( BLINK_PIN, blink_on );
        blink_on = !blink_on;
        sleep_ms( 10 );

    }
}
