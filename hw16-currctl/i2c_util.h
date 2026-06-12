
#ifndef I2C_UTIL_H__
#define I2C_UTIL_H__

#include "pin.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace I2CUtil
{
    static constexpr i2c_inst_t *PORT { i2c1 };

    static constexpr uint BAUD_KHZ { 400 };
    static constexpr uint BAUD_HZ { BAUD_KHZ * 1000 };

    static inline void init( void ); 

    static inline void read_device(
        const std::uint8_t dev_addr, const std::uint8_t *reg_addr,
        std::uint8_t *buf, const std::size_t len
    );

    static inline void write_device(
        const std::uint8_t dev_addr, const std::uint8_t *reg_addr,
        const std::uint8_t *buf, const std::size_t len
    );

}

static inline void
I2CUtil::init( void )
{
    i2c_init( PORT, BAUD_HZ );

    gpio_set_function( Pin::I2C_SDA, GPIO_FUNC_I2C );
    gpio_pull_up( Pin::I2C_SDA );

    gpio_set_function( Pin::I2C_SCL, GPIO_FUNC_I2C );
    gpio_pull_up( Pin::I2C_SCL );
}


static inline void
I2CUtil::read_device( 
    const std::uint8_t dev_addr, const std::uint8_t *reg_addr,
    std::uint8_t *buf, const std::size_t len
)
{
    i2c_write_blocking( PORT, dev_addr, reg_addr, 1, true );
    i2c_read_blocking( PORT, dev_addr, buf, len, false );
}

static inline void
I2CUtil::write_device(
    const std::uint8_t dev_addr, const std::uint8_t *reg_addr,
    const std::uint8_t *buf, const std::size_t len
)
{
    i2c_write_blocking( PORT, dev_addr, reg_addr, 1, true );
    i2c_write_blocking( PORT, dev_addr, buf, len, false );
}

#endif
