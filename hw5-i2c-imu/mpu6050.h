
#ifndef MPU6050_H__
#define MPU6050_H__

#include "i2c_util.h"
#include "hardware/i2c.h"
#include <cstdint>
#include <array>
#include <bit>
#include <cstring>
#include <cstddef>

namespace MPU6050
{
    // I2C address of the MPU6050
    static constexpr std::uint8_t I2C_ADDR { 0x68 };

    using sensor_buf_t = std::array< std::uint8_t, 14 >;

    namespace SensorId
    {
        static constexpr std::size_t
            ACCEL_X { 0 },
            ACCEL_Y { 1 },
            ACCEL_Z { 2 },
            TEMP    { 3 },
            GYRO_X  { 4 },
            GYRO_Y  { 5 },
            GYRO_Z  { 6 }
            ;
    }

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
        std::int16_t
            accel_x {}, accel_y {}, accel_z {},
            temp {},
            gyro_x {}, gyro_y {}, gyro_z {};
    
        SensorData( void ) = default;

        SensorData( const sensor_buf_t & );
    };

    static inline void init( void );
    static inline std::uint8_t read_whoami( void );
    static inline SensorData read_sensor( void );
}

MPU6050::
SensorData::SensorData( const sensor_buf_t &buf ) :
    accel_x { i16_from_u8buf( &buf.data()[ 0 ] ) },
    accel_y { i16_from_u8buf( &buf.data()[ 2 ] ) },
    accel_z { i16_from_u8buf( &buf.data()[ 4 ] ) },

    temp    { i16_from_u8buf( &buf.data()[ 6 ] ) },

    gyro_x  { i16_from_u8buf( &buf.data()[ 8 ] ) },
    gyro_y  { i16_from_u8buf( &buf.data()[ 10 ] ) },
    gyro_z  { i16_from_u8buf( &buf.data()[ 12 ] ) }
{}

// Function to initialize the MPU6050
static inline void
MPU6050::init( void )
{
    // Wake up the MPU6050 by writing 0 to the PWR_MGMT_1 register
    std::uint8_t val { 0x00 };

    write_i2c_device( MPU6050::I2C_ADDR, &Regs::PWR_MGMT_1, &val, 1 );

    static constexpr std::uint8_t DLPF_CFG { 0x4 }; // 20Hz bandwidth
    val = DLPF_CFG;
    write_i2c_device( MPU6050::I2C_ADDR, &Regs::CONFIG, &val, 1 );

    static constexpr std::uint8_t AFS_SEL { 0x0 }; // +-2g
    val = AFS_SEL << 3;
    write_i2c_device( MPU6050::I2C_ADDR, &Regs::ACCEL_CONFIG, &val, 1 );

    static constexpr std::uint8_t GYRO_FS_SEL { 0x3 }; // +-2000deg/s
    val = GYRO_FS_SEL << 3;
    write_i2c_device( MPU6050::I2C_ADDR, &Regs::GYRO_CONFIG, &val, 1 );
}

static inline std::uint8_t
MPU6050::read_whoami( void )
{
    std::uint8_t whoami {};
    read_i2c_device( MPU6050::I2C_ADDR, &Regs::WHO_AM_I, &whoami, 1 );
    return whoami;
}

// Function to read sensor data
static inline MPU6050::SensorData
MPU6050::read_sensor( void )
{
    sensor_buf_t sensorbuf {};
    read_i2c_device(
        MPU6050::I2C_ADDR,
        &Regs::ACCEL_XOUT_H,
        sensorbuf.data(),
        sensorbuf.size() / sizeof( std::uint8_t )
    );
    return SensorData { sensorbuf };
}

#endif
