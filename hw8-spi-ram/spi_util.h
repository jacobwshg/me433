
#ifndef __SPI_UTIL_H
#define __SPI_UTIL_H

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <cstddef>
#include <cstdint>

// SPI Defines

#define SPI_PORT spi1

namespace Pins
{
    static constexpr uint
        SPI_MISO    { 8  },
        SPI_CSn_DAC { 9  },
        SPI_SCK     { 10 },
        SPI_MOSI    { 11 },
        
        SPI_CSn_RAM { 13 }
        ;
}

namespace SPIUtil
{
    namespace Aux
    {
        static inline void do_nop( void );
        static inline void write_cs( const uint, const bool );
    }

    static inline void cs_select( const uint );
    static inline void cs_deselect( const uint );
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
SPIUtil::Aux::write_cs( const uint pin, const bool val )
{
    do_nop();
    gpio_put( pin, val ? 1 : 0 );
    do_nop();
}

static inline void
SPIUtil::cs_select( const uint pin )
{
    Aux::write_cs( pin, 0 );
}

static inline void
SPIUtil::cs_deselect( const uint pin )
{
    Aux::write_cs( pin, 1 );
}


#endif // __SPI_UTIL_H
