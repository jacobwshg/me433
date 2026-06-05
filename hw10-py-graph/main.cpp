
#include "i2c_util.h"
#include "mpu6050.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <cstdio>

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments


int main()
{
    stdio_init_all();

    I2CUtil::init();

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
