
#ifndef __SPI_UTIL_H
#define __SPI_UTIL_H

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <cstddef>
#include <cstdint>

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0

namespace Pins
{
    static constexpr uint
        SPI_MISO { 16 },
        SPI_CS   { 17 },
        SPI_SCK  { 18 },
        SPI_MOSI { 19 }
        ;
}

namespace SPIUtil
{
    namespace Aux
    {
        static inline void do_nop( void );
        static inline void write_cs( const bool );
    }

    static inline void cs_select( void );
    static inline void cs_deselect( void );
}

static inline void
SPIUtil::Aux::do_nop( void )
{
    asm volatile(
        "nop\n"
        "nop\n"
        "nop\n"
    );
}

static inline void
SPIUtil::Aux::write_cs( const bool val )
{
    do_nop();
    gpio_put( Pins::SPI_CS, val ? 1 : 0 );
    do_nop();
}

static inline void
SPIUtil::cs_select( void )
{
    Aux::write_cs( 0 );
}

static inline void
SPIUtil::cs_deselect( void )
{
    Aux::write_cs( 1 );
}


#endif // __SPI_UTIL_H
