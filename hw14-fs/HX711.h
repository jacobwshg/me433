
#include "pico/stdlib.h"
#include <cstdint>
#include <cstdio>

namespace HX711
{
    static constexpr uint PIN_DT { 15 };
    static constexpr uint PIN_SCK { 16 };

    static inline void init( void );
    static inline void wait_sample_ready( void );
    static inline std::int32_t read_sample( void );
    static inline void reset_gain( void );

    static constexpr std::uint64_t SLEEP_INTERVAL { 20 };
}

static inline void
HX711::init( void )
{
    gpio_init( PIN_DT );
    gpio_set_dir( PIN_DT, GPIO_IN );
    gpio_pull_up( PIN_DT );

    gpio_init( PIN_SCK );
    gpio_set_dir( PIN_SCK, GPIO_OUT );
    
    gpio_put( PIN_SCK, 0 );
}

static inline void
HX711::wait_sample_ready( void )
{
    while ( 1 == gpio_get( PIN_DT ) )
    {
        //std::printf( "Collecting data\n" );
        sleep_us( SLEEP_INTERVAL );
    }
}

static inline void
HX711::reset_gain( void )
{   
    gpio_put( PIN_SCK, 1 );
    sleep_us( SLEEP_INTERVAL );
    gpio_put( PIN_SCK, 0 );
    sleep_us( SLEEP_INTERVAL );

}

static inline std::int32_t
HX711::read_sample( void )
{
    std::int32_t dout { 0 }; 
    for ( std::size_t i { 0 }; i < 24; ++i )
    {
        gpio_put( PIN_SCK, 1 );
        sleep_us( SLEEP_INTERVAL );
        dout = ( dout << 1 ) | !!gpio_get( PIN_DT );
        gpio_put( PIN_SCK, 0 );
        sleep_us( SLEEP_INTERVAL );
    }

    reset_gain();

    // sign extend
    if ( 1 & ( dout >> 23 ) )
    {
        dout |= ( ( ~0 ) << 24 );
    }
    return dout;
}
