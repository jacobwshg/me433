
#ifndef SSD1306_H__
#define SSD1306_H__

#include "font.h"
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

    static constexpr std::size_t WIDTH { 128 }, HEIGHT { 32 };
    static constexpr std::size_t BUFLEN { ( WIDTH * HEIGHT ) / 8 + 1 };


    static constexpr std::uint8_t I2C_ADDR { 0b0111100 }; // 7bit i2c address
    static inline std::array< std::uint8_t, BUFLEN > buffer {}; // 128x32/8. Every bit is a pixel except first byte

    // convert x,y pixel coordinates to byte index in the buffer
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

    // send a command instruction (not pixel data)
    static inline void
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

    // update every pixel on the screen
    static inline void
    update()
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

    // zero every pixel value
    static inline void
    clear()
    {
        SSD1306::buffer.fill( 0x00 ); // make every bit a 0, memset in string.h
        SSD1306::buffer[ 0 ] = 0x40; // first byte is part of command
    }


    static inline void
    setup( void )
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


    //
    // set a pixel value. Call update() to push to the display)
    // actually unused when drawing chars, because we draw one byte-aligned column at a time
    //
    static inline void
    drawpixel(
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

    // draw a column of pixels within a character
    static inline void
    draw_char_col(
        const std::size_t x, const std::size_t y,
        std::uint8_t col_pxs
    )
    {
        // round down y values to nearest multiple of 8
        const std::size_t buf_byte_idx { SSD1306::xy_to_byte_idx( x, y ) };
        SSD1306::buffer[ buf_byte_idx ] = col_pxs;
    }

    // draw a character
    static inline void
    draw_char( const char c, const std::size_t x, const std::size_t y )
    {
        if ( ( c < 0x20 ) || ( c > 0x7F ) ) { return; }

        const ascii_bm_t &char_bm { ASCII[ c - 0x20 ] };
        for ( std::size_t dx = 0; dx < char_bm.size(); ++dx )
        {
            SSD1306::draw_char_col( x + dx, y, char_bm[ dx ] );
        }
    }

    // draw a string of characters within the display bounds
    static inline void
    draw_msg( const std::string_view msg, const std::size_t x0, const std::size_t y0 )
    {
        std::size_t x { x0 }, y { y0 };
        for ( std::size_t i = 0; i < msg.size(); ++i )
        {
            if ( x+6 >= SSD1306::WIDTH )
            {
                x = 0;
                y += 8;
            }
            if ( y >= SSD1306::HEIGHT )
            {
                break;
            }
            SSD1306::draw_char( msg[ i ], x, y );
            x += 6;
        }
    }

    static inline void
    draw_crosshair( void )
    {
        static constexpr std::size_t
            xc { SSD1306::WIDTH / 2 },
            yc ( SSD1306::HEIGHT / 2 );

        for ( std::size_t x { xc-3 }; x < xc+4; ++x )
        {
            drawpixel( x, yc, true );
        }
        draw_char_col( xc, yc-1, 0b1110'0000 );
        draw_char_col( xc, yc,   0b0000'1111 );
    }

}
#endif
