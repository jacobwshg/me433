
#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"

int main()
{
    stdio_init_all();

    while ( true )
    {
        printf( "Hello, world!\n" );
        sleep_ms( 1000 );
    }
}
