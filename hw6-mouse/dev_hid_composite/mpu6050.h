
#ifndef MPU6050_H__
#define MPU6050_H__

#include "util.h"
#include "i2c_util.h"
#include "hardware/i2c.h"
#include <stdint.h>
#include <string.h>
#include <stddef.h>

static const uint16_t MPU6050_I2C_ADDR = 0x68;

typedef uint8_t MPU6050_sensor_buf_t[ 14 ];

// enum MPU6050_Sensor_ID
// {
//     ACCEL_X { 0 },
//     ACCEL_Y { 1 },
//     ACCEL_Z { 2 },
//     TEMP    { 3 },
//     GYRO_X  { 4 },
//     GYRO_Y  { 5 },
//     GYRO_Z  { 6 },
// };

enum MPU6050_Reg
{
    //
    // config
    //
    CONFIG       = 0x1A,
    GYRO_CONFIG  = 0x1B,
    ACCEL_CONFIG = 0x1C,
    PWR_MGMT_1   = 0x6B,
    PWR_MGMT_2   = 0x6C,

    //
    // sensor data
    //
    ACCEL_XOUT_H = 0x3B,
    ACCEL_XOUT_L = 0x3C,
    ACCEL_YOUT_H = 0x3D,
    ACCEL_YOUT_L = 0x3E,
    ACCEL_ZOUT_H = 0x3F,
    ACCEL_ZOUT_L = 0x40,

    TEMP_OUT_H = 0x41,
    TEMP_OUT_L = 0x42,

    GYRO_XOUT_H = 0x43,
    GYRO_XOUT_L = 0x44,
    GYRO_YOUT_H = 0x45,
    GYRO_YOUT_L = 0x46,
    GYRO_ZOUT_H = 0x47,
    GYRO_ZOUT_L = 0x48,
    
    WHO_AM_I = 0x75,
};

struct MPU6050_sensor_data
{
    int16_t
        accel_x, accel_y, accel_z,
        temp,
        gyro_x, gyro_y, gyro_z;
    
};

static inline void MPU6050_sensor_data_init(
    struct MPU6050_sensor_data *sensordata,
    const MPU6050_sensor_buf_t sensorbuf
);
static inline void MPU6050_init( void );
static inline uint8_t MPU6050_read_whoami( void );
static inline void MPU6050_read_sensor( struct MPU6050_sensor_data *sensordata );

static inline void
MPU6050_sensor_data_init(
    struct MPU6050_sensor_data *sensordata,
    const MPU6050_sensor_buf_t sensorbuf
)
{
    if ( !sensordata ) { return; }

    sensordata->accel_x = i16_from_u8buf( &sensorbuf[ 0 ] );
    sensordata->accel_y = i16_from_u8buf( &sensorbuf[ 2 ] );
    sensordata->accel_z = i16_from_u8buf( &sensorbuf[ 4 ] );
    
    sensordata->temp = i16_from_u8buf( &sensorbuf[ 6 ] );

    sensordata->gyro_x = i16_from_u8buf( &sensorbuf[ 8 ] );
    sensordata->gyro_y = i16_from_u8buf( &sensorbuf[ 10 ] );
    sensordata->gyro_z = i16_from_u8buf( &sensorbuf[ 12 ] );
}

// Function to initialize the MPU6050
static inline void
MPU6050_init( void )
{
    // Wake up the MPU6050 by writing 0 to the PWR_MGMT_1 register
    uint8_t val = 0x00;

    uint8_t reg = ( uint8_t ) PWR_MGMT_1;

    write_i2c_device( MPU6050_I2C_ADDR, &reg, &val, 1 );

    static const uint8_t DLPF_CFG = 0x4; // 20Hz bandwidth
    val = DLPF_CFG;
    reg = ( uint8_t ) CONFIG;
    write_i2c_device( MPU6050_I2C_ADDR, &reg, &val, 1 );

    static const uint8_t AFS_SEL = 0x0; // +-2g
    val = AFS_SEL << 3;
    reg = ( uint8_t ) ACCEL_CONFIG;
    write_i2c_device( MPU6050_I2C_ADDR, &reg, &val, 1 );

    static const uint8_t GYRO_FS_SEL = 0x3; // +-2000deg/s
    val = GYRO_FS_SEL << 3;
    reg = ( uint8_t ) GYRO_CONFIG;
    write_i2c_device( MPU6050_I2C_ADDR, &reg, &val, 1 );
}

static inline uint8_t
MPU6050_read_whoami( void )
{
    uint8_t whoami;
    static const uint8_t reg = ( uint8_t ) WHO_AM_I;
    read_i2c_device( MPU6050_I2C_ADDR, &reg, &whoami, 1 );
    return whoami;
}

// Function to read sensor data
static inline void
MPU6050_read_sensor( struct MPU6050_sensor_data *sensordata )
{
    MPU6050_sensor_buf_t sensorbuf;
    static const uint8_t reg = ( uint8_t ) ACCEL_XOUT_H;
    read_i2c_device(
        MPU6050_I2C_ADDR,
        &reg,
        ( uint8_t * ) sensorbuf,
        sizeof sensorbuf / sizeof( uint8_t )
    );
    MPU6050_sensor_data_init( sensordata, sensorbuf );
}

#endif
