
#ifndef SPI_UTIL_H__
#define SPI_UTIL_H__

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <cstddef>
#include <cstdint>

// SPI Defines

#define SPI_PORT spi1

namespace Pins
{
    static constexpr uint
        SPI_MISO { 8  },
        SPI_CS   { 9  },
        SPI_SCK  { 10 },
        SPI_MOSI { 11 }
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
