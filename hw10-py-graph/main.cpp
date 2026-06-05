
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
    MPU6050::init();

    while ( true )
    {
        MPU6050::SensorData data { MPU6050::read_sensor() };

        const std::int16_t
            gyro_x { data.gyro_x },
            gyro_y { data.gyro_y },
            gyro_z { data.gyro_z };

        std::printf( "Gyro (x, y, z): %d, %d, %d\n", gyro_x, gyro_y, gyro_z );

        sleep_ms( 100 );
    }
}
