
#include "HX711.h"
#include "pico/stdlib.h"
#include <cstdio>

namespace Pins
{
    static constexpr uint
        ADC_DT  { 15 },
        ADC_SCK { 16 };
}

int main()
{
    stdio_init_all();

    printf( "Hello, world!\n" );

    HX711::init();

    while ( true )
    {
        HX711::wait_sample_ready();
        const std::int32_t dout { HX711::read_sample() };
        std::printf( "sample: %08d\n", dout );
    }
}
