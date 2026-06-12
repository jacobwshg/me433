
#ifndef UTIL_H__
#define UTIL_H__

#include <array>
#include <cstdint>

namespace Util
{
    std::uint16_t U8_MSK { 0xff };

    static inline std::uint16_t u16_from_u8s( const std::uint8_t * );
    static inline std::array< std::uint8_t, 2 > u8s_from_u16( const std::uint16_t );
        
    static inline std::int16_t i16_from_u8s( const std::uint8_t * );
    static inline std::array< std::uint8_t, 2 > u8s_from_i16( const std::int16_t );
}

static inline std::uint16_t
Util::u16_from_u8s( const std::uint8_t *buf )
{
    static constexpr std::size_t HI { 0 }, LO { 1 };
    return ( static_cast< std::uint16_t >( buf[ HI ] ) << 8 )
        | ( static_cast< std::uint16_t >( buf[ LO ] ) ); 
}


static inline std::array< std::uint8_t, 2 >
Util::u8s_from_u16( const std::uint16_t val )
{
    std::array< std::uint8_t, 2 > buf {};
    buf[ 0 ] = static_cast< std::uint8_t >( ( val >> 8 ) & U8_MSK );
    buf[ 1 ] = static_cast< std::uint8_t >( val & U8_MSK );
    return buf;
}

static inline std::int16_t
Util::i16_from_u8s( const std::uint8_t *buf )
{
    return static_cast< std::int16_t >( u16_from_u8s( buf ) ); 
}

static inline std::array< std::uint8_t, 2 >
Util::u8s_from_i16( const std::int16_t val )
{
    return u8s_from_u16( static_cast< std::uint16_t >( val ) );
}
    
#endif
