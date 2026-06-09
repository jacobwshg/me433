
#ifndef UTIL_H__
#define UTIL_H__

#include <stdint.h>

static inline int16_t
i16_from_u8buf( const uint8_t *buf )
{
    enum { HI = 0 , LO = 1 };
    return ( ( ( int16_t )buf[ HI ] ) << 8 ) | ( int16_t )buf[ LO ]; 
}

#endif
