
#ifndef I2C_DEF_H
#define I2C_DEF_H

#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <array>
#include <cstddef>
#include <cstdint>

static constexpr i2c_inst_t *I2C_PORT { i2c0 };
static constexpr std::uint8_t
    I2C_SDA_PIN { 20 },
    I2C_SCL_PIN { 21 }
    ;

static inline void
read_i2c_device( 
    const std::uint8_t dev_addr, const std::uint8_t reg_addr,
    std::uint8_t *buf, const std::size_t len
)
{
    i2c_write_blocking( I2C_PORT, dev_addr, &reg_addr, 1, true );
    i2c_read_blocking( I2C_PORT, dev_addr, buf, len, false );
}

static inline void
write_i2c_device(
    const std::uint8_t dev_addr, const std::uint8_t reg_addr,
    const std::uint8_t *buf, const std::size_t len
)
{
    i2c_write_blocking( I2C_PORT, dev_addr, &reg_addr, 1, true );
    i2c_write_blocking( I2C_PORT, dev_addr, buf, len, false );
}

#endif
