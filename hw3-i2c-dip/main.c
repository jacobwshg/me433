
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5

#define HEARTBEAT_PIN 20

#define MCP_LED_OUT_PIN 7
#define MCP_BTN_IN_PIN  0

enum MCPRegId
{
    MCP_IODIR   = 0x00,
    MCP_IPOL    = 0x01,
    MCP_GPINTEN = 0x02,
    MCP_DEFVAL  = 0x03,
    MCP_INTCON  = 0x04,
    MCP_IOCON   = 0x05,
    MCP_GPPU    = 0x06,
    MCP_INTF    = 0x07,
    MCP_INTCAP  = 0x08,
    MCP_GPIO    = 0x09,
    MCP_OLAT    = 0x0a,
};

#define MCP_I2C_ADDR_6_3 0x20 // 0b010'0xxx
#define MCP_I2C_ADDR_2_0 0x07 // custom: 0b111
#define MCP_I2C_ADDR ( MCP_I2C_ADDR_6_3 | MCP_I2C_ADDR_2_0 )

static void
heartbeat_init( void )
{
    gpio_init( HEARTBEAT_PIN );
    gpio_set_dir( HEARTBEAT_PIN, GPIO_OUT );
}


static void
set_heartbeat( void )
{
    static const absolute_time_t
        HB_MAX_ONTIME = 100000, HB_MAX_OFFTIME = 900000;

    static absolute_time_t
        now = 0, state_endtime = 0;

    enum HBState
    {
        HB_ON, HB_OFF,
    };
    static enum HBState hb_state = HB_OFF;

    switch ( hb_state )
    {
    case HB_ON:
        now = get_absolute_time();
        if ( now >= state_endtime )
        {
            gpio_put( HEARTBEAT_PIN, false );
            hb_state = HB_OFF;
            state_endtime = now + HB_MAX_OFFTIME;
        }
        break;
    case HB_OFF:
        now = get_absolute_time();
        if ( now >= state_endtime )
        {
            gpio_put( HEARTBEAT_PIN, true );
            hb_state = HB_ON;
            state_endtime = now + HB_MAX_ONTIME;
        }
        break;
    default:
        break;
    }

}

void
write_i2c_device(
    const uint8_t addr,
    const uint8_t reg, const uint8_t val
)
{
    bool nostop;
    uint8_t buf[ 2 ];
    buf[ 0 ] = reg; buf[ 1 ] = val;
    i2c_write_blocking(
        I2C_PORT,
        addr,
        buf, 2,
        nostop=false
    );
}

uint8_t
read_i2c_device(
    const uint8_t addr, 
    const uint8_t reg
)
{
    uint8_t buf;
    bool nostop;
    i2c_write_blocking(
        I2C_PORT,
        addr,
        &reg, 1,
        nostop=true
    );
    i2c_read_blocking(
        I2C_PORT,
        addr,
        &buf, 1,
        nostop=false
    );
    return buf;
}


static void
mcp_init( void )
{
    uint8_t mcp_iodir_val = 0x0;
    // register MCP button input pin
    mcp_iodir_val |= ( 0x1 << MCP_BTN_IN_PIN );
    write_i2c_device(
        MCP_I2C_ADDR, MCP_IODIR, mcp_iodir_val
    );
}

int
main()
{
    stdio_init_all();

    heartbeat_init();

    //

    // I2C Initialisation. Using it at 400Khz.
    i2c_init( I2C_PORT, 400*1000 );
    
    gpio_set_function( I2C_SDA_PIN, GPIO_FUNC_I2C );
    gpio_set_function( I2C_SCL_PIN, GPIO_FUNC_I2C );
    //gpio_pull_up( I2C_SDA_PIN );
    //gpio_pull_up( I2C_SCL_PIN );
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    mcp_init();

    while ( true )
    {
        set_heartbeat();

        const uint8_t mcp_gpio_val = read_i2c_device(
            MCP_I2C_ADDR,
            MCP_GPIO
        );
        const uint8_t btn_val = 0x1 & ( mcp_gpio_val >> MCP_BTN_IN_PIN );

        uint8_t mcp_olat_val = 0x0;
        if ( btn_val )
        {
            mcp_olat_val |= ( 0x1 << MCP_LED_OUT_PIN );
        }
        write_i2c_device(
            MCP_I2C_ADDR,
            MCP_OLAT, mcp_olat_val
        );

    }

}
