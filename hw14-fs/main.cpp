
#include "HX711.h"
#include "pico/stdlib.h"
#include <cstdio>

namespace IIR
{
    static constexpr float A { 0.3F };
    static constexpr float B { 0.7F };
    float avg { 0.0F };

    static inline float iir( const float cur )
    {
        return avg = A * avg + B * cur;
    }
}

int main()
{
    stdio_init_all();

    printf( "Hello, world!\n" );

    HX711::init();

    // sleep_ms( 10000 );
    // FILE *fp { std::fopen( "data.txt", "w" ) };
    // if ( fp )
    // {
    //     std::printf( "open successful\n" );
    //     std::fclose( fp );
    // } else { std::printf( "open failed\n" ); }

    while ( true )
    {
        HX711::wait_sample_ready();
        const std::int32_t dout { HX711::read_sample() };

        const float dout_f { static_cast< float >( dout ) };
        const float iir_avg_f { IIR::iir( dout_f ) };
        const std::int32_t iir_avg { static_cast< std::int32_t >( iir_avg_f ) };

        const absolute_time_t us_since_boot { get_absolute_time() };
        const std::uint32_t ms_since_boot { to_ms_since_boot( us_since_boot ) };
        std::printf( "%9d ( filtered %9d ) @ %u ms\n", dout, iir_avg, ms_since_boot );
    }
}
