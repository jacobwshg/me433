
#ifndef MPU6050_H__
#define MPU6050_H__

#include "i2c_def.h"
#include "hardware/i2c.h"
#include "hardware/i2c.h"
#include <cstdint>
#include <array>
#include <bit>
#include <cstring>

namespace MPU6050
{
    // I2C address of the MPU6050
    static constexpr std::uint8_t I2C_ADDR { 0x68 };

    // Register addresses
    namespace Regs
    {
        static constexpr std::uint8_t
            //
            // config
            //
            CONFIG       { 0x1A },
            GYRO_CONFIG  { 0x1B },
            ACCEL_CONFIG { 0x1C },
            PWR_MGMT_1   { 0x6B },
            PWR_MGMT_2   { 0x6C },

            //
            // sensor data
            //
            ACCEL_XOUT_H { 0x3B },
            ACCEL_XOUT_L { 0x3C },
            ACCEL_YOUT_H { 0x3D },
            ACCEL_YOUT_L { 0x3E },
            ACCEL_ZOUT_H { 0x3F },
            ACCEL_ZOUT_L { 0x40 },

            TEMP_OUT_H { 0x41 },
            TEMP_OUT_L { 0x42 },

            GYRO_XOUT_H { 0x43 },
            GYRO_XOUT_L { 0x44 },
            GYRO_YOUT_H { 0x45 },
            GYRO_YOUT_L { 0x46 },
            GYRO_ZOUT_H { 0x47 },
            GYRO_ZOUT_L { 0x48 },
    
            WHO_AM_I { 0x75 }
            ;
    }

    struct SensorData
    {
        std::int16_t accel_x {};
        std::int16_t accel_y {};
        std::int16_t accel_z {};

        std::int16_t temp {};

        std::int16_t gyro_x {};
        std::int16_t gyro_y {};
        std::int16_t gyro_z {};
    };

    // Function to initialize the MPU6050
    static inline void init( void )
    {
        // Wake up the MPU6050 by writing 0 to the PWR_MGMT_1 register
        std::array< std::uint8_t, 2 > buf { Regs::PWR_MGMT_1, 0x00 };
        i2c_write_blocking( I2C_PORT, MPU6050::I2C_ADDR, buf.data(), buf.size(), false );
        buf[ 0 ] = Regs::ACCEL_CONFIG;
        i2c_write_blocking( I2C_PORT, MPU6050::I2C_ADDR, buf.data(), buf.size(), false );
        buf[ 0 ] = Regs::GYRO_CONFIG;
        i2c_write_blocking( I2C_PORT, MPU6050::I2C_ADDR, buf.data(), buf.size(), false );
    }

    static inline std::uint8_t read_whoami( void )
    {
        std::uint8_t whoami {};
        i2c_write_blocking( I2C_PORT, MPU6050::I2C_ADDR, &Regs::WHO_AM_I, 1, true );
        i2c_read_blocking( I2C_PORT, MPU6050::I2C_ADDR, &whoami, 1, false );
        return whoami;
    }

    // Function to read sensor data
    static inline void read_sensor( struct SensorData &sensor_data )
    {
        std::array< std::uint8_t, 14 > buf {};
        i2c_write_blocking( I2C_PORT, MPU6050::I2C_ADDR, &Regs::ACCEL_XOUT_H, 1, true );
        i2c_read_blocking( I2C_PORT, MPU6050::I2C_ADDR, buf.data(), buf.size(), false );

        sensor_data.accel_x = *( std::bit_cast< std::int16_t * >( &buf[ 0 ] ) );
        sensor_data.accel_y = *( std::bit_cast< std::int16_t * >( &buf[ 2 ] ) );
        sensor_data.accel_z = *( std::bit_cast< std::int16_t * >( &buf[ 4 ] ) );
        sensor_data.temp    = *( std::bit_cast< std::int16_t * >( &buf[ 6 ] ) );
        sensor_data.gyro_x  = *( std::bit_cast< std::int16_t * >( &buf[ 8 ] ) );
        sensor_data.gyro_y  = *( std::bit_cast< std::int16_t * >( &buf[ 10 ] ) );
        sensor_data.gyro_z  = *( std::bit_cast< std::int16_t * >( &buf[ 12 ] ) );
    }

}

#endif
