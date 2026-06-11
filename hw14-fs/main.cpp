
#include "HX711.h"
#include "pico/stdlib.h"
#include <cstdio>
#include <array>

namespace IIR
{
    static constexpr float A { 0.9F };
    static constexpr float B { 1.0F - A };
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

        const absolute_time_t uptime_us { get_absolute_time() };
        const std::uint32_t uptime_ms { to_ms_since_boot( uptime_us ) };
        std::printf( "%d,%d,%u\n", dout, iir_avg, uptime_ms );
    }
}
