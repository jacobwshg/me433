
#ifndef AS5600_H__
#define AS5600_H__

#include "util.h"
#include "i2c_util.h"

#include <cstdint>
#include <cstddef>
#include <array>

namespace AS5600
{
    static constexpr std::uint8_t I2C_ADDR { 0x36 };

    namespace Reg
    {
        static constexpr std::uint8_t
            ZMCO { 0x00 },
            ZPOS_H { 0x01 },
            ZPOS_L { 0x02 },
            MPOS_H { 0x03 },
            MPOS_L { 0x04 },
            MANG_H { 0x05 },
            MANG_L { 0x06 },
            CONF_H { 0x07 },
            CONF_L { 0x08 },

            RAW_ANGLE_H { 0x0c },
            RAW_ANGLE_L { 0x0d },
            ANGLE_H { 0x0e },
            ANGLE_L { 0x0f },

            STATUS { 0x0b },
            AGC { 0x1a },
            MAG_H { 0x1b },
            MAG_L { 0x1c },
            
            BURN { 0xff };

        static inline std::uint8_t to_hi( const std::uint8_t reg )
        {
            switch( reg )
            {
            case ZPOS_L: return ZPOS_H; break;
            case MPOS_L: return MPOS_H; break;
            case MANG_L: return MANG_H; break;
            case CONF_L: return CONF_H; break;
            case RAW_ANGLE_L: return RAW_ANGLE_H; break;
            case ANGLE_L: return ANGLE_H; break;
            case MAG_L: return MAG_H; break;
            default: return reg; break; 
            }
        }

        static inline bool is_8bit( const std::uint8_t reg_ )
        {
            const std::uint8_t reg { to_hi( reg_ ) };
            return
            {
                reg == Reg::ZMCO ||
                reg == Reg::STATUS ||
                reg == Reg::AGC ||
                reg == Reg::BURN
            };
        }

    }

    static inline std::uint16_t readreg( const std::uint8_t reg );
    static inline void writereg( const std::uint8_t reg, const std::uint16_t val );

}

static inline std::uint16_t 
AS5600::readreg( const std::uint8_t reg )
{
    const std::size_t len { Reg::is_8bit( reg ) ? 1UL : 2UL }; 
    std::array< std::uint8_t, 2 > buf {};
    I2CUtil::read_device( AS5600::I2C_ADDR, &reg, buf.data(), len );
    if ( 1 == len )
    {
        buf[ 1 ] = buf[ 0 ];
        buf[ 0 ] = 0;
    }
    return Util::u16_from_u8s( buf.data() );
}

static inline void
AS5600::writereg( const std::uint8_t reg, const std::uint16_t val )
{
    const std::size_t len { Reg::is_8bit( reg ) ? 1UL : 2UL }; 
    std::array< std::uint8_t, 2 > buf { Util::u8s_from_u16( val ) };
    if ( 1 == len )
    {
        buf[ 0 ] = buf[ 1 ];
        buf[ 1 ] = 0;
    }
    I2CUtil::write_device( AS5600::I2C_ADDR, &reg, buf.data(), len );
}


#endif
