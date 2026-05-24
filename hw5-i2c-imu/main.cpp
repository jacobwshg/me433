
#include "mpu6050.h"
#include "ssd1306.h"
#include "i2c_def.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <cstdio>
#include <cstring>

// I2C defines
// This example will use I2C0 on GPIO4 (SDA) and GPIO5 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_SDA 4
#define I2C_SCL 5

#define BLINK_PIN 0

int main()
{

    bool blink_on { true };

    stdio_init_all();

    gpio_init( BLINK_PIN );
    gpio_set_dir( BLINK_PIN, GPIO_OUT );

    // I2C Initialisation. Using it at 400Khz.
    i2c_init( I2C_PORT, 400*1000 );
    
    gpio_set_function( I2C_SDA, GPIO_FUNC_I2C );
    gpio_set_function( I2C_SCL, GPIO_FUNC_I2C );
    gpio_pull_up( I2C_SDA );
    gpio_pull_up( I2C_SCL );
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    SSD1306::setup(); SSD1306::clear(); SSD1306::update();
    SSD1306::draw_crosshair();
    SSD1306::update();

    MPU6050::init();

    while ( true )
    {
        gpio_put( BLINK_PIN, blink_on );
        blink_on = !blink_on;
        sleep_ms( 500 );
    }
}
