
#include "spi_util.h"
#include "mcp4912.h"

#include "pico/stdlib.h"
#include "hardware/spi.h"

#include <cstdio>


namespace Pins
{

}

int
main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init( SPI_PORT, 1000*1000 );
    gpio_set_function( Pins::SPI_MISO, GPIO_FUNC_SPI );
    gpio_set_function( Pins::SPI_CS,   GPIO_FUNC_SIO );
    gpio_set_function( Pins::SPI_SCK,  GPIO_FUNC_SPI );
    gpio_set_function( Pins::SPI_MOSI, GPIO_FUNC_SPI );
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir( Pins::SPI_CS, GPIO_OUT );
    gpio_put( Pins::SPI_CS, 1 );
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    while ( true )
    {
        std::printf( "Hello, world!\n" );
        sleep_ms( 1000 );
    }
}
