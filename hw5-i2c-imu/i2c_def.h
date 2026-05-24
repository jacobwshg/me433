
#ifndef I2C_DEF_H
#define I2C_DEF_H

#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <array>

static constexpr i2c_inst_t *I2C_PORT { i2c0 };
static constexpr uint
    I2C_SDA_PIN { 4 },
    I2C_SCL_PIN { 5 }
    ;

#endif
