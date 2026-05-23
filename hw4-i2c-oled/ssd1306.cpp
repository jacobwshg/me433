
// based on adafruit and sparkfun libraries

#include "ssd1306.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <array>
#include <cstdint>
#include <cstddef>
#include <cstring>


namespace SSD1306
{
    static constexpr std::uint8_t I2C_ADDR { 0b0111100 }; // 7bit i2c address
    static std::array< std::uint8_t, 513 > buffer; // 128x32/8. Every bit is a pixel except first byte
}

// send a command instruction (not pixel data)
static void
ssd1306_command( const std::uint8_t c )
{
    //i2c_master_start();
    //i2c_master_send(ssd1306_write);
    //i2c_master_send(0x00); // bit 7 is 0 for Co bit (data bytes only), bit 6 is 0 for DC (data is a command))
    //i2c_master_send(c);
    //i2c_master_stop();

    std::array< std::uint8_t, 2> buf { 0x00, c }; // bit 7 is 0 for Co bit (data bytes only), bit 6 is 0 for DC (data is a command))
    i2c_write_blocking(
        i2c_default,
        SSD1306::I2C_ADDR,
        buf.data(), 2,
        false
    );
}

void
ssd1306_setup( void )
{
    // first byte in ssd1306_buffer is a command
    SSD1306::buffer[ 0 ] = 0x40;
    // give a little delay for the ssd1306 to power up
    //_CP0_SET_COUNT(0);
    //while (_CP0_GET_COUNT() < 48000000 / 2 / 50) {
    //}
    sleep_ms( 20 );
    ssd1306_command( SSD1306_DISPLAYOFF );
    ssd1306_command( SSD1306_SETDISPLAYCLOCKDIV );
    ssd1306_command( 0x80 );
    ssd1306_command( SSD1306_SETMULTIPLEX );
    ssd1306_command( 0x1F ); // height-1 = 31
    ssd1306_command( SSD1306_SETDISPLAYOFFSET );
    ssd1306_command( 0x0 );
    ssd1306_command( SSD1306_SETSTARTLINE );
    ssd1306_command( SSD1306_CHARGEPUMP );
    ssd1306_command( 0x14 );
    ssd1306_command( SSD1306_MEMORYMODE );
    ssd1306_command( 0x00 );
    ssd1306_command( SSD1306_SEGREMAP | 0x1 );
    ssd1306_command( SSD1306_COMSCANDEC );
    ssd1306_command( SSD1306_SETCOMPINS );
    ssd1306_command( 0x02 );
    ssd1306_command( SSD1306_SETCONTRAST );
    ssd1306_command( 0x8F );
    ssd1306_command( SSD1306_SETPRECHARGE );
    ssd1306_command( 0xF1 );
    ssd1306_command( SSD1306_SETVCOMDETECT );
    ssd1306_command( 0x40 );
    ssd1306_command( SSD1306_DISPLAYON );
    SSD1306::buffer[ 0 ] = 0x40;
    ssd1306_clear();
    ssd1306_update();
}

// update every pixel on the screen
void ssd1306_update()
{
    ssd1306_command( SSD1306_PAGEADDR );
    ssd1306_command( 0 );
    ssd1306_command( 0xFF );
    ssd1306_command( SSD1306_COLUMNADDR );
    ssd1306_command( 0 );
    ssd1306_command( 128 - 1 ); // Width

    unsigned short count = 512; // WIDTH * ((HEIGHT + 7) / 8)
    unsigned char * ptr = SSD1306::buffer.data(); // first address of the pixel buffer
    /*
    i2c_master_start();
    i2c_master_send(ssd1306_write);
    i2c_master_send(0x40); // send pixel data
    // send every pixel
    while (count--) {
        i2c_master_send(*ptr++);
    }
    i2c_master_stop();
    */

    i2c_write_blocking( i2c_default, SSD1306::I2C_ADDR, ptr, 513, false );
}

// set a pixel value. Call update() to push to the display)
void
ssd1306_draw_pixel(
    const std::size_t x, const std::size_t y,
    bool color
)
{
    static constexpr std::size_t
        MAX_IROW = 32, MAX_ICOL = 128;

    if (
        ( x < 0 ) || ( x >= MAX_ICOL ) ||
        ( y < 0 ) || ( y >= MAX_IROW ) )
    {
        return;
    }

    //
    // byte 0 in buf is command,
    // pixel bytes start at idx 1
    //
    const std::size_t buf_byte_idx = 1 + ( y / 8 ) * MAX_ICOL + x;

    if ( color )
    {
        SSD1306::buffer[ buf_byte_idx ] |= ( 1 << ( y & 7 ) );
    }
    else
    {
        SSD1306::buffer[ buf_byte_idx ] &= ~( 1 << ( y & 7 ) );
    }
}

// zero every pixel value
void
ssd1306_clear()
{
    SSD1306::buffer.fill( 0x00 ); // make every bit a 0, memset in string.h
    SSD1306::buffer[ 0 ] = 0x40; // first byte is part of command
}

