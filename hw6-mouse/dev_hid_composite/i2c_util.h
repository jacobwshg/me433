
#ifndef I2C_UTIL_H
#define I2C_UTIL_H

#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include <stddef.h>
#include <stdint.h>

#define I2C_PORT i2c0

#define I2C_BAUD_KHZ 400
#define I2C_BAUD_HZ ( I2C_BAUD_KHZ * 1000 )

#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5

static inline void
my_i2c_init( void )
{
    i2c_init( I2C_PORT, I2C_BAUD_HZ );
    gpio_set_function( I2C_SDA_PIN, GPIO_FUNC_I2C ); gpio_pull_up( I2C_SDA_PIN );
    gpio_set_function( I2C_SCL_PIN, GPIO_FUNC_I2C ); gpio_pull_up( I2C_SCL_PIN );

}

static inline void
read_i2c_device( 
    const uint8_t dev_addr, const uint8_t *reg_addr,
    uint8_t *buf, const size_t len
)
{
    i2c_write_blocking( I2C_PORT, dev_addr, reg_addr, 1, true );
    i2c_read_blocking( I2C_PORT, dev_addr, buf, len, false );
}

static inline void
write_i2c_device(
    const uint8_t dev_addr, const uint8_t *reg_addr,
    const uint8_t *buf, const size_t len
)
{
    i2c_write_blocking( I2C_PORT, dev_addr, reg_addr, 1, true );
    i2c_write_blocking( I2C_PORT, dev_addr, buf, len, false );
}

#endif
