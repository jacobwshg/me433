
#ifndef __SPI_UTIL_H
#define __SPI_UTIL_H

#include "pico/stdlib.h"
#include <cstddef>
#include <cstdint>

namespace SPIUtil
{
    namespace Aux
    {
        static inline void do_nop( void );
        static inline void write_cs( const uint, const std::uint8_t );
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
SPIUtil::Aux::write_cs( const uint cs_pin, const std::uint8_t val )
{
    do_nop();
    gpio_put( cs_pin, val ? 1 : 0 );
    do_nop();
}

static inline void
SPIUtil::cs_select( const uint cs_pin )
{
    Aux::write_cs( cs_pin, 0 );
}

static inline void
SPIUtil::cs_deselect( const uint cs_pin )
{
    Aux::write_cs( cs_pin, 1 );
}


#endif // __SPI_UTIL_H
