
#ifndef __MCP4912_H
#define __MCP4912_H

#include "spi_util.h"
#include <cstddef>
#include <algorithm>

namespace MCP4912
{
    static constexpr std::size_t DWIDTH { 10 };
    static constexpr std::uint16_t DATA_MSK { ( 0b1 << DWIDTH ) - 1 };

    static constexpr std::size_t DMAX { 1 << DWIDTH };
    static constexpr float DMAX_F { static_cast<float>( DMAX ) };
    static constexpr float INV_DMAX_F { 1.0F / DMAX_F };
    
    static constexpr std::size_t CMD_WIDTH { 4 };

    enum class Channel: std::uint16_t { A = 0, B = 1 };

    static constexpr std::size_t
        CHAN_OFS  { 15 },
        BUF_OFS   { 14 },
        NGA_OFS   { 13 },
        NSHDN_OFS { 12 },
        DATA_OFS  { 12 - DWIDTH }
        ;
    static constexpr std::uint16_t
        // wire to high - input buffered
        BUF   { 0b1 }, 
         // wire to high - gain = 1x
        NGA   { 0b1 },
        // wire to high - output enabled 
        NSHDN { 0b1 }  
        ;

    static constexpr float GAIN_F { NGA ? 1.0F : 2.0F };

    static constexpr float VREF_F { 3.3F };
    static constexpr float INV_VREF_F { 1.0F / VREF_F };

    static inline std::uint16_t make_command( const Channel chan );

    static inline void write( const Channel, const std::uint16_t );

    static inline std::uint16_t Vout_from_f( const float );
}

static inline std::uint16_t
MCP4912::make_command( const Channel chan )
{
    return ( ( BUF<<BUF_OFS ) | ( NGA<<NGA_OFS ) | ( NSHDN<<NSHDN_OFS ) )
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
    SPIUtil::cs_select();
    spi_write16_blocking( SPI_PORT, &pkt, 1 );
    SPIUtil::cs_deselect();
}

static inline std::uint16_t
MCP4912::Vout_from_f( const float raw_Vout_f )
{
    const float Vout_f { std::clamp( raw_Vout_f, 0.0F, VREF_F ) };
    const std::uint16_t Vout
    {
        static_cast< std::uint16_t >( Vout_f * INV_VREF_F * DMAX_F )
    };
    return Vout;
}

#endif // __MCP4912_H
