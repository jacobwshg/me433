
#include "spi_util.h"
#include "mcp4912.h"

#include "pico/stdlib.h"
#include "hardware/spi.h"

//#include <math.h>
#include <cstdio>
#include <cmath>
#include <array>

namespace Pins
{

}

namespace
{
    static constexpr std::uint32_t BAUD_MHZ { 1 };
    static constexpr std::uint32_t KHZ_PER_MHZ { 1000 };
    static constexpr std::uint32_t BAUD_KHZ { BAUD_MHZ * KHZ_PER_MHZ };
    static constexpr std::uint32_t BAUD_HZ { BAUD_KHZ * 1000 };

    // sine and triangle wave frequencies
    static constexpr std::uint32_t
        SIN_HZ { 2 },
        TRI_HZ { 1 }
        ;
    // upd. rate
    static constexpr std::uint32_t
        UPD_HZ { 200 };
    static constexpr float INV_UPD_HZ { 1.0F / UPD_HZ };
    static constexpr absolute_time_t
        SLEEP_UNTIL_DT { 1'000'000 / UPD_HZ };

    static constexpr std::uint32_t
        SIN_SAMPLES_PER_PERIOD { UPD_HZ / SIN_HZ },
        TRI_SAMPLES_PER_PERIOD { UPD_HZ / TRI_HZ };

    static inline std::array< std::uint16_t, SIN_SAMPLES_PER_PERIOD > SIN_PERIOD_VOUT_CACHE {};
    static inline std::array< std::uint16_t, TRI_SAMPLES_PER_PERIOD > TRI_PERIOD_VOUT_CACHE {};

    static constexpr float TWO_PI { M_PI * 2.0F };
    // increment in radian of each sin wave sample
    static constexpr float SIN_SAMPLE_DRAD { TWO_PI / SIN_SAMPLES_PER_PERIOD };

    // absolute increment in the vertical direction of each triangle wave sample
    // numerator 2.0: total positive 1.0 in former half period + total negative 1.0 in latter half period 
    static constexpr float TRI_SAMPLE_DY_ABS { 2.0F / TRI_SAMPLES_PER_PERIOD };
}

void
init_Vout_cache( void )
{
    float rad { 0.0F };
    for ( std::uint16_t &Vout : ( ::SIN_PERIOD_VOUT_CACHE ) )
    {
        rad += ::SIN_SAMPLE_DRAD;
        float sin_val { std::sin( rad ) };
        // map from [ -1, 1 ] to [ 0, 1 ]
        sin_val = 0.5F + ( 0.5F * sin_val ); 
        Vout = MCP4912::Vout_from_scale( sin_val );
    }

    float y { 0.0F };
    std::size_t idx { 0 };
    for ( std::uint16_t &Vout : ( ::TRI_PERIOD_VOUT_CACHE ) )
    {
        if ( idx < TRI_SAMPLES_PER_PERIOD / 2 )
        {
            y += ::TRI_SAMPLE_DY_ABS;
        }
        else { y -= ::TRI_SAMPLE_DY_ABS; }
        Vout = MCP4912::Vout_from_scale( y );
        ++idx;
    }
}

int
main()
{
    stdio_init_all();

    init_Vout_cache();

    //spi_set_format( SPI_PORT, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST );

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init( SPI_PORT, ::BAUD_HZ );
    gpio_set_function( Pins::SPI_MISO, GPIO_FUNC_SPI );
    gpio_set_function( Pins::SPI_CS,   GPIO_FUNC_SIO );
    gpio_set_function( Pins::SPI_SCK,  GPIO_FUNC_SPI );
    gpio_set_function( Pins::SPI_MOSI, GPIO_FUNC_SPI );
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir( Pins::SPI_CS, GPIO_OUT );
    gpio_put( Pins::SPI_CS, 1 );
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    std::size_t i_sin { 0 }, i_tri { 0 };

    // MCP4912::write( MCP4912::Channel::A, MCP4912::Vout_from_f( 0.0F ) );
    // MCP4912::write( MCP4912::Channel::B, MCP4912::Vout_from_f( 1.0F ) );

    while ( true )
    {
        const absolute_time_t now { get_absolute_time() };

        MCP4912::write( MCP4912::Channel::A, ::SIN_PERIOD_VOUT_CACHE[ i_sin ] );
        MCP4912::write( MCP4912::Channel::B, ::TRI_PERIOD_VOUT_CACHE[ i_tri ] );
        // MCP4912::write( MCP4912::Channel::A, static_cast<std::uint16_t>( 0U ) ); // test
        // MCP4912::write( MCP4912::Channel::B, MCP4912::DMAX ); // test
        if ( ++i_sin == SIN_SAMPLES_PER_PERIOD ) { i_sin = 0; }
        if ( ++i_tri == TRI_SAMPLES_PER_PERIOD ) { i_tri = 0; }
        sleep_until( now + ::SLEEP_UNTIL_DT );

    }
}
