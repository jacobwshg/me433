
#ifndef INA219_H__
#define INA219_H__

#include "util.h"
#include "i2c_util.h"
#include "hardware/i2c.h"
#include <cstdint>

namespace INA219
{
    namespace Reg
    {
        static constexpr std::uint8_t
            CONFIG  { 0x00 },
            VSHUNT  { 0x01 },
            VBUS    { 0x02 },
            POWER   { 0x03 },
            CURRENT { 0x04 },
            CALIB   { 0x05 };
    }

    static constexpr std::uint8_t I2C_ADDR { 0b0100'0000 };

    static inline void init( void );

    static inline std::uint16_t read( const std::uint8_t reg );
    static inline std::int16_t read_current_raw( void );
    static inline std::int16_t read_vshunt( void );

    static inline void write( const std::uint8_t reg, const std::uint16_t val );

    static inline float tomA( const std::int16_t val )
    {
        return ( 1.0F / 3 ) * static_cast< float >( val );
    }

}

static inline void
INA219::init( void )
{
    static constexpr std::uint16_t cfg { 0b0011'0000'1000'1111 };
    static constexpr std::uint16_t cal { 1024 };

    INA219::write( Reg::CONFIG, cfg );

    INA219::write( Reg::CALIB,  cal );

}

static inline std::uint16_t
INA219::read( const std::uint8_t reg )
{
    std::array< std::uint8_t, 2 > buf {};
    I2CUtil::read_device(
        INA219::I2C_ADDR, &reg, buf.data(), 2
    );
    return Util::u16_from_u8s( buf.data() );
}

static inline std::int16_t
INA219::read_current_raw( void )
{
    return static_cast< std::int16_t >( INA219::read( Reg::CURRENT ) );
}

static inline std::int16_t
INA219::read_vshunt( void )
{
    return static_cast< std::int16_t >( INA219::read( Reg::VSHUNT) );
}

static inline void
INA219::write( const std::uint8_t reg, const std::uint16_t val )
{
    //std::array< std::uint8_t, 2 > buf { Util::u8s_from_u16( val ) };
    //I2CUtil::write_device( INA219::I2C_ADDR, &reg, buf.data(), 2 );

    // INA219 requires single { reg, val } write 
    std::array< std::uint8_t, 2 > valbuf { Util::u8s_from_u16( val ) };
    std::array< std::uint8_t, 3 > msg { reg, valbuf[ 0 ], valbuf[ 1 ] };
    i2c_write_blocking( I2CUtil::PORT, INA219::I2C_ADDR, msg.data(), 3, false );
}

#endif
