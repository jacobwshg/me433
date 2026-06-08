
#include "spi_util.h"
#include "mcp4912.h"
#include "mcp_23k256.h"

#include "pico/stdlib.h"
#include "hardware/spi.h"

//#include <math.h>
#include <cstdio>
#include <cmath>
#include <array>


namespace
{
    static constexpr std::uint32_t BAUD_KHZ { 12 };
    static constexpr std::uint32_t BAUD_HZ { BAUD_KHZ * 1000 };

    // sine wave frequency
    static constexpr std::uint32_t SIN_HZ { 1 };
    // upd. rate
    static constexpr std::uint32_t
        UPD_HZ { 1000 };
    static constexpr float INV_UPD_HZ { 1.0F / UPD_HZ };
    static constexpr absolute_time_t
        SLEEP_UNTIL_DT { 1'000'000 / UPD_HZ };

    static constexpr std::uint32_t
        SIN_SAMPLES_PER_PERIOD { UPD_HZ / SIN_HZ };

    static constexpr float TWO_PI { M_PI * 2.0F };
    // increment in radian of each sin wave sample
    static constexpr float SIN_SAMPLE_DRAD { TWO_PI / SIN_SAMPLES_PER_PERIOD };

}

void
write_sine_cache( void )
{
    static constexpr std::uint16_t RAM_BASE_ADDR { 0x0 };
    MCP_23K256::begin_seqwrite( Pins::SPI_CSn_RAM, RAM_BASE_ADDR );

    for ( std::size_t i { 0 }; i<::SIN_SAMPLES_PER_PERIOD; ++i )
    {
        float rad { static_cast< float >( i ) * ::SIN_SAMPLE_DRAD };
        float sin_val { std::sin( rad ) };
        // map from [ -1, 1 ] to [ 0, 1 ]
        sin_val = 0.5F + ( 0.5F * sin_val ); 
        const std::uint16_t Vout { MCP4912::Vout_from_scale( sin_val ) };
        std::printf( "writing sin sample %zu: %04u\n", i, Vout );

        MCP_23K256::seqwrite_u16( Vout );
    }

    MCP_23K256::end_seqwrite( Pins::SPI_CSn_RAM );
}

int
main()
{
    stdio_init_all();

    // SPI init
    spi_init( SPI_PORT, ::BAUD_HZ );
    spi_set_format( SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST );

    gpio_set_function( Pins::SPI_MISO, GPIO_FUNC_SPI );
    //gpio_set_function( Pins::SPI_CS,   GPIO_FUNC_SIO );
    gpio_set_function( Pins::SPI_SCK,  GPIO_FUNC_SPI );
    gpio_set_function( Pins::SPI_MOSI, GPIO_FUNC_SPI );

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init( Pins::SPI_CSn_DAC );
    gpio_set_dir( Pins::SPI_CSn_DAC, GPIO_OUT );
    gpio_put( Pins::SPI_CSn_DAC, 1 );

    gpio_init( Pins::SPI_CSn_RAM );
    gpio_set_dir( Pins::SPI_CSn_RAM, GPIO_OUT );
    gpio_put( Pins::SPI_CSn_RAM, 1 );
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    // test reference voltage
    MCP4912::write( Pins::SPI_CSn_DAC, MCP4912::Channel::A, MCP4912::Vout_from_f( 3.3F ) );
    MCP4912::write( Pins::SPI_CSn_DAC, MCP4912::Channel::B, MCP4912::Vout_from_f( 3.3F ) );
    sleep_ms( 10000 );

    write_sine_cache();

    std::uint16_t ram_rd_addr { 0x0 };

    while ( true )
    {
        const absolute_time_t now { get_absolute_time() };

        MCP_23K256::begin_seqread( Pins::SPI_CSn_RAM, ram_rd_addr );
        const std::uint16_t Vout_sin { MCP_23K256::seqread_u16() };
        MCP_23K256::end_seqread( Pins::SPI_CSn_RAM );
        ram_rd_addr += 2;

        std::printf( "read sin Vout: %04u\n", Vout_sin );

        MCP4912::write( Pins::SPI_CSn_DAC, MCP4912::Channel::B, Vout_sin );

        //sleep_until( now + ::SLEEP_UNTIL_DT );
        sleep_ms( 1000 );

    }
}
