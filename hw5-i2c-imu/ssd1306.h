#ifndef SSD1306_H__
#define SSD1306_H__

#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <array>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string_view>

// Based on the adafruit and sparkfun libraries
#define SSD1306_MEMORYMODE          0x20 
#define SSD1306_COLUMNADDR          0x21 
#define SSD1306_PAGEADDR            0x22 
#define SSD1306_SETCONTRAST         0x81 
#define SSD1306_CHARGEPUMP          0x8D 
#define SSD1306_SEGREMAP            0xA0 
#define SSD1306_DISPLAYALLON_RESUME 0xA4 
#define SSD1306_NORMALDISPLAY       0xA6 
#define SSD1306_INVERTDISPLAY       0xA7 
#define SSD1306_SETMULTIPLEX        0xA8 
#define SSD1306_DISPLAYOFF          0xAE 
#define SSD1306_DISPLAYON           0xAF 
#define SSD1306_COMSCANDEC          0xC8 
#define SSD1306_SETDISPLAYOFFSET    0xD3 
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5 
#define SSD1306_SETPRECHARGE        0xD9 
#define SSD1306_SETCOMPINS          0xDA 
#define SSD1306_SETVCOMDETECT       0xDB 
#define SSD1306_SETSTARTLINE        0x40 
#define SSD1306_DEACTIVATE_SCROLL   0x2E ///< Stop scroll

namespace SSD1306
{

    namespace Cmd
    {
        static constexpr std::uint8_t
            MEMORYMODE { 0x20 },
            COLUMNADDR { 0x21 },
            PAGEADDR { 0x22 },
            SETCONTRAST { 0x81 },
            CHARGEPUMP { 0x8D },
            SEGREMAP { 0xA0 },
            DISPLAYALLON_RESUME { 0xA4 },
            NORMALDISPLAY { 0xA6 },
            INVERTDISPLAY { 0xA7 },
            SETMULTIPLEX { 0xA8 },
            DISPLAYOFF { 0xAE },
            DISPLAYON { 0xAF },
            COMSCANDEC { 0xC8 },
            SETDISPLAYOFFSET { 0xD3 },
            SETDISPLAYCLOCKDIV { 0xD5 },
            SETPRECHARGE { 0xD9 },
            SETCOMPINS { 0xDA },
            SETVCOMDETECT { 0xDB },
            SETSTARTLINE { 0x40 },
            DEACTIVATE_SCROLL { 0x2E }
            ;
    }

    static constexpr std::uint8_t I2C_ADDR { 0b0111100 }; // 7bit i2c address

    static constexpr std::size_t WIDTH { 128 }, HEIGHT { 32 };
    static constexpr std::size_t BUFLEN { ( WIDTH * HEIGHT ) / 8 + 1 };

    static std::array< std::uint8_t, BUFLEN > buffer {}; // 128x32/8. Every bit is a pixel except first byte

    static inline std::size_t xy_to_byte_idx(
        const std::size_t x, const std::size_t y
    )
    {
        //
        // byte 0 in buf is command,
        // pixel bytes start at idx 1
        // pixels are packed in bytes vertically
        //
        std::size_t byte_idx { 1 + ( y / 8 ) * SSD1306::WIDTH + x };
        return byte_idx;
    }

    static void command( const std::uint8_t );

    void setup( void );
    void update( void );
    void drawpixel( const std::size_t, const std::size_t, bool );
    void clear( void );

}

/// this should be private
// send a command instruction (not pixel data)
static void
SSD1306::command( const std::uint8_t c )
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
SSD1306::setup( void )
{
    // first byte in ssd1306_buffer is a command
    SSD1306::buffer[ 0 ] = 0x40;
    // give a little delay for the ssd1306 to power up
    //_CP0_SET_COUNT(0);
    //while (_CP0_GET_COUNT() < 48000000 / 2 / 50) {
    //}
    sleep_ms( 20 );
    SSD1306::command( Cmd::CHARGEPUMP );
    SSD1306::command( Cmd::SETDISPLAYCLOCKDIV );
    SSD1306::command( 0x80 );
    SSD1306::command( Cmd::SETMULTIPLEX );
    SSD1306::command( 0x1F ); // height-1 = 31
    SSD1306::command( Cmd::SETDISPLAYOFFSET );
    SSD1306::command( 0x0 );
    SSD1306::command( Cmd::SETSTARTLINE );
    SSD1306::command( Cmd::CHARGEPUMP );
    SSD1306::command( 0x14 );
    SSD1306::command( Cmd::MEMORYMODE );
    SSD1306::command( 0x00 );
    SSD1306::command( Cmd::SEGREMAP | 0x1 );
    SSD1306::command( Cmd::COMSCANDEC );
    SSD1306::command( Cmd::SETCOMPINS );
    SSD1306::command( 0x02 );
    SSD1306::command( Cmd::SETCONTRAST );
    SSD1306::command( 0x8F );
    SSD1306::command( Cmd::SETPRECHARGE );
    SSD1306::command( 0xF1 );
    SSD1306::command( Cmd::SETVCOMDETECT );
    SSD1306::command( 0x40 );
    SSD1306::command( Cmd::DISPLAYON );
    SSD1306::buffer[ 0 ] = 0x40;
    SSD1306::clear();
    SSD1306::update();
}

// update every pixel on the screen
void
SSD1306::update()
{
    SSD1306::command( Cmd::PAGEADDR );
    SSD1306::command( 0 );
    SSD1306::command( 0xFF );
    SSD1306::command( Cmd::COLUMNADDR );
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

//
// set a pixel value. Call update() to push to the display)
// actually unused when drawing chars, because we draw one byte-aligned column at a time
//
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

    const std::size_t buf_byte_idx { SSD1306::xy_to_byte_idx( x, y ) };
    if ( color )
    {
        SSD1306::buffer[ buf_byte_idx ] |= ( 1 << ( y % 8 ) );
    }
    else
    {
        SSD1306::buffer[ buf_byte_idx ] &= ~( 1 << ( y % 8 ) );
    }
}

// zero every pixel value
void
SSD1306::clear()
{
    SSD1306::buffer.fill( 0x00 ); // make every bit a 0, memset in string.h
    SSD1306::buffer[ 0 ] = 0x40; // first byte is part of command
}

#endif
