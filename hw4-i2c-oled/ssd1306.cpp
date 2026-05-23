
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
    static std::array< std::uint8_t, BUFLEN > buffer; // 128x32/8. Every bit is a pixel except first byte

    // send a command instruction (not pixel data)
    static void
    command( const std::uint8_t c )
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
}

void
SSD1306::setup( void )
{
    // first byte in ssd1306_buffer is a command
    SSD1306::buffer[ 0 ] = 0x40;
    // give a little delay for the ssd1306 to power up
    //_CP0_SET_COUNT(0);
    //while (_CP0_GET_COUNT() < 48000000 / 2 / 50) {
    //}
    sleep_ms( 20 );
    SSD1306::command( SSD1306_DISPLAYOFF );
    SSD1306::command( SSD1306_SETDISPLAYCLOCKDIV );
    SSD1306::command( 0x80 );
    SSD1306::command( SSD1306_SETMULTIPLEX );
    SSD1306::command( 0x1F ); // height-1 = 31
    SSD1306::command( SSD1306_SETDISPLAYOFFSET );
    SSD1306::command( 0x0 );
    SSD1306::command( SSD1306_SETSTARTLINE );
    SSD1306::command( SSD1306_CHARGEPUMP );
    SSD1306::command( 0x14 );
    SSD1306::command( SSD1306_MEMORYMODE );
    SSD1306::command( 0x00 );
    SSD1306::command( SSD1306_SEGREMAP | 0x1 );
    SSD1306::command( SSD1306_COMSCANDEC );
    SSD1306::command( SSD1306_SETCOMPINS );
    SSD1306::command( 0x02 );
    SSD1306::command( SSD1306_SETCONTRAST );
    SSD1306::command( 0x8F );
    SSD1306::command( SSD1306_SETPRECHARGE );
    SSD1306::command( 0xF1 );
    SSD1306::command( SSD1306_SETVCOMDETECT );
    SSD1306::command( 0x40 );
    SSD1306::command( SSD1306_DISPLAYON );
    SSD1306::buffer[ 0 ] = 0x40;
    SSD1306::clear();
    SSD1306::update();
}

// update every pixel on the screen
void SSD1306::update()
{
    SSD1306::command( SSD1306_PAGEADDR );
    SSD1306::command( 0 );
    SSD1306::command( 0xFF );
    SSD1306::command( SSD1306_COLUMNADDR );
    SSD1306::command( 0 );
    SSD1306::command( 128 - 1 ); // Width

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

    i2c_write_blocking( i2c_default, SSD1306::I2C_ADDR, SSD1306::buffer.data(), SSD1306::BUFLEN, false );
}

// set a pixel value. Call update() to push to the display)
void
SSD1306::drawpixel(
    const std::size_t x, const std::size_t y,
    bool color
)
{
    if (
        ( x < 0 ) || ( x >= SSD1306::WIDTH ) ||
        ( y < 0 ) || ( y >= SSD1306::HEIGHT ) )
    {
        return;
    }

    //
    // byte 0 in buf is command,
    // pixel bytes start at idx 1
    //
    const std::size_t buf_byte_idx = 1 + ( y / 8 ) * SSD1306::WIDTH + x;

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
SSD1306::clear()
{
    SSD1306::buffer.fill( 0x00 ); // make every bit a 0, memset in string.h
    SSD1306::buffer[ 0 ] = 0x40; // first byte is part of command
}

