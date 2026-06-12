
#include "pin.h"

#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include <cstdio>

int main()
{
    stdio_init_all();

    while ( true )
    {
        std::printf( "Hello, world!\n" );
        sleep_ms( 1000 );
    }
}
