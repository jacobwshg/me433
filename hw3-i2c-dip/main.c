
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

enum MCPOpcode 
{
    IODIR   = 0x00,
    IPOL    = 0x01,
    GPINTEN = 0x02,
    DEFVAL  = 0x03,
    INTCON  = 0x04,
    IOCON   = 0x05,
    GPPU    = 0x06,
    INTF    = 0x07,
    INTCAP  = 0x08,
    GPIO    = 0x09,
    OLAT    = 0x0a,
};

typedef uint8_t mcp_addr_t; // only 7 bits are used

static const mcp_addr_t MCP_ADDR_DEFAULT = 0x20; // 010'0xxx

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
        HB_INIT, HB_ON, HB_OFF,
    } ;
    static enum HBState hb_state = HB_INIT;

    switch ( hb_state )
    {
    case HB_INIT:
        gpio_put( HEARTBEAT_PIN, true );
        hb_state = HB_ON;
        now = get_absolute_time();
        state_endtime = now + HB_MAX_ONTIME;
        break;
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
    gpio_pull_up( I2C_SDA_PIN );
    gpio_pull_up( I2C_SCL_PIN );
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    while ( true )
    {
        set_heartbeat();
        //printf( "Hello, world!\n" );
        //sleep_ms( 1000 );
    }

}
