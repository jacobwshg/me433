
#ifndef I2C_DEF_H
#define I2C_DEF_H

#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace I2CUtil
{
    static constexpr i2c_inst_t *PORT { i2c0 };

    static constexpr uint BAUD_KHZ { 400 };

    static constexpr uint
        PIN_SDA { 4 },
        PIN_SCL { 5 };

    static inline void init( void );

    static inline std::int16_t i16_from_u8buf( const std::uint8_t * );

    static inline void read_i2c_device(
        const std::uint8_t, const std::uint8_t *,
        std::uint8_t *, const std::size_t
    );

    static inline void write_i2c_device(
        const std::uint8_t, const std::uint8_t *,
        const std::uint8_t *, const std::size_t
    );
}

static inline void
I2CUtil::init( void )
{
    i2c_init( PORT, BAUD_KHZ*1000 );
    
    gpio_set_function( PIN_SDA, GPIO_FUNC_I2C );
    gpio_set_function( PIN_SCL, GPIO_FUNC_I2C );
    gpio_pull_up( PIN_SDA );
    gpio_pull_up( PIN_SCL );
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

}

static inline std::int16_t
I2CUtil::i16_from_u8buf( const std::uint8_t *buf )
{
    static constexpr std::size_t HI { 0 }, LO { 1 };
    return ( static_cast< std::int16_t >( buf[ HI ] ) << 8 )
        | ( static_cast< std::int16_t >( buf[ LO ] ) ); 
}

static inline void
I2CUtil::read_i2c_device( 
    const std::uint8_t dev_addr, const std::uint8_t *reg_addr,
    std::uint8_t *buf, const std::size_t len
)
{
    i2c_write_blocking( PORT, dev_addr, reg_addr, 1, true );
    i2c_read_blocking( PORT, dev_addr, buf, len, false );
}

static inline void
I2CUtil::write_i2c_device(
    const std::uint8_t dev_addr, const std::uint8_t *reg_addr,
    const std::uint8_t *buf, const std::size_t len
)
{
    i2c_write_blocking( PORT, dev_addr, reg_addr, 1, true );
    i2c_write_blocking( PORT, dev_addr, buf, len, false );
}

#endif
