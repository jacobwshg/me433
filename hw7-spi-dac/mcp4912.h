
#ifndef __MCP4912_H
#define __MCP4912_H

#include "spi_util.h"
#include <cstddef>
#include <algorithm>
#include <cstring>
#include <array>

namespace MCP4912
{
    static constexpr std::size_t DWIDTH { 10 };
    static constexpr std::uint16_t DMAX { ( 1 << DWIDTH ) - 1 };
    static constexpr float DMAX_F { static_cast< float >( DMAX ) };
    static constexpr float INV_DMAX_F { 1.0F / DMAX_F };
    static constexpr std::uint16_t DATA_MSK { DMAX };

    enum class Channel: std::uint16_t { A = 0, B = 1 };

    static constexpr std::size_t
        CHAN_OFS  { 15 },
        BUF_OFS   { 14 },
        GAn_OFS   { 13 },
        SHDNn_OFS { 12 },
        DATA_OFS  { 12 - DWIDTH }
        ;
    static constexpr std::uint16_t
        // wire to high - input buffered
        BUF   { 0b1 }, 
         // wire to high - gain = 1x
        GAn   { 0b1 },
        // wire to high - output enabled 
        SHDNn { 0b1 }  
        ;

    static constexpr float GAIN_F { GAn ? 1.0F : 2.0F };

    static constexpr float VREF_F { 3.3F };
    static constexpr float INV_VREF_F { 1.0F / VREF_F };

    static inline std::uint16_t make_command( const Channel chan );

    static inline void write( const Channel, const std::uint16_t );

    //
    // make a 16-bit Vout value from a raw scale between 0 and 1
    //
    static inline std::uint16_t Vout_from_scale( const float );
    //
    // make a 16-bit Vout value from a raw value between 0 and 3.3V
    //
    static inline std::uint16_t Vout_from_f( const float );

}

static inline std::uint16_t
MCP4912::make_command( const Channel chan )
{
    return ( ( BUF<<BUF_OFS ) | ( GAn<<GAn_OFS ) | ( SHDNn<<SHDNn_OFS ) )
        | ( static_cast< std::uint16_t >( chan ) << CHAN_OFS );
}

static inline void
MCP4912::write(  const Channel chan, const std::uint16_t data )
{
    const std::uint16_t pkt
    {
        make_command( chan )
        | ( ( data & DATA_MSK ) << DATA_OFS )
    };
    
    static std::array< std::uint8_t, 2 > buf {};
    buf[ 0 ] = static_cast<std::uint8_t>( pkt >> 8 );
    buf[ 1 ] = static_cast<std::uint8_t>( pkt & 0xffU );

    SPIUtil::cs_select();
    spi_write_blocking( SPI_PORT, buf.data(), buf.size() );
    SPIUtil::cs_deselect();
}

static inline std::uint16_t
MCP4912::Vout_from_scale( const float raw_scale_f )
{
    const float scale_f { std::clamp( raw_scale_f, 0.0F, 1.0F ) };
    const std::uint16_t Vout { static_cast< std::uint16_t >( scale_f * DMAX_F ) };
    return Vout;
}

static inline std::uint16_t
MCP4912::Vout_from_f( const float raw_Vout_f )
{
    const float Vout_f { std::clamp( raw_Vout_f, 0.0F, VREF_F ) };
    const float scale_f { Vout_f * INV_VREF_F };
    const std::uint16_t Vout { Vout_from_scale( scale_f ) };
    return Vout;
}

#endif // __MCP4912_H
