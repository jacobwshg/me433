
#include "pin.h"
#include "AS5600.h"
#include "i2c_util.h"

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include <cstdio>

// // I2C defines
// // This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// // Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
// #define I2C_PORT i2c0
// #define I2C_SDA 8
// #define I2C_SCL 9

int main()
{
    stdio_init_all();

    I2CUtil::init();

    sleep_ms( 3000 );
    std::printf( "I2C init done\n" );

    while ( true )
    {
        static std::uint16_t rawangle {}, angle {};
        rawangle = AS5600::readreg( AS5600::Reg::RAW_ANGLE_H );
        //angle = AS5600::readreg( AS5600::Reg::ANGLE_H );
        //std::printf( "%u,%u\n", rawangle, angle );
        std::printf( "%u\n", rawangle );
        sleep_ms( 100 );
    }
}
