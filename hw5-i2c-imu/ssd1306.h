
#ifndef SSD1306_H__
#define SSD1306_H__

#include "font.h"
#include "i2c_def.h"
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
    using px_pos_t = std::int16_t;

    static constexpr std::uint8_t I2C_ADDR { 0b0111100 }; // 7bit i2c address

    static constexpr px_pos_t WIDTH { 128 }, HEIGHT { 32 };
    static constexpr px_pos_t BUFLEN { ( WIDTH * HEIGHT ) / 8 + 1 };

    static constexpr px_pos_t
        XC { SSD1306::WIDTH / 2 },
        YC ( SSD1306::HEIGHT / 2 );

    static inline std::array< std::uint8_t, BUFLEN > buffer {}; // 128x32/8. Every bit is a pixel except first byte

    // convert x,y pixel coordinates to byte index in the buffer
    static inline px_pos_t xy_to_byte_idx(
        const px_pos_t x, const px_pos_t y
    )
    {
        //
        // byte 0 in buf is command,
        // pixel bytes start at idx 1
        // pixels are packed in bytes vertically
        //
        px_pos_t byte_idx { 1 + ( y / 8 ) * SSD1306::WIDTH + x };
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
            I2C_PORT,
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

        i2c_write_blocking( I2C_PORT, SSD1306::I2C_ADDR, SSD1306::buffer.data(), SSD1306::BUFLEN, false );
    }

    // zero every pixel value
    static inline void
    clear()
    {
        SSD1306::buffer.fill( 0x00 ); // make every bit a 0, memset in string.h
        SSD1306::buffer[ 0 ] = 0x40; // first byte is part of command
    }


    static inline void
    init( void )
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
        const px_pos_t x, const px_pos_t y,
        bool color
    )
    {
        if (
            ( x < 0 ) || ( x >= SSD1306::WIDTH ) ||
            ( y < 0 ) || ( y >= SSD1306::HEIGHT ) )
        {
            return;
        }

        const px_pos_t buf_byte_idx { SSD1306::xy_to_byte_idx( x, y ) };
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
        const px_pos_t x, const px_pos_t y,
        std::uint8_t col_pxs
    )
    {
        // round down y values to nearest multiple of 8
        const px_pos_t buf_byte_idx { SSD1306::xy_to_byte_idx( x, y ) };
        SSD1306::buffer[ buf_byte_idx ] = col_pxs;
    }

    // draw a character
    static inline void
    draw_char( const char c, const px_pos_t x, const px_pos_t y )
    {
        if ( ( c < 0x20 ) || ( c > 0x7F ) ) { return; }

        const ascii_bm_t &char_bm { ASCII[ c - 0x20 ] };
        for ( px_pos_t dx = 0; dx < char_bm.size(); ++dx )
        {
            SSD1306::draw_char_col( x + dx, y, char_bm[ dx ] );
        }
    }

    // draw a string of characters within the display bounds
    static inline void
    draw_msg( const std::string_view msg, const px_pos_t x0, const px_pos_t y0 )
    {
        px_pos_t x { x0 }, y { y0 };
        for ( px_pos_t i = 0; i < msg.size(); ++i )
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

        for ( px_pos_t x { XC-3 }; x < XC+4; ++x )
        {
            drawpixel( x, YC, true );
        }
        draw_char_col( XC, YC-1, 0b1110'0000 );
        draw_char_col( XC, YC,   0b0000'1111 );
    }

    static inline void
    draw_segment(
        const px_pos_t x0, const px_pos_t y0,
        const px_pos_t x1, const px_pos_t y1
    )
    {
        const px_pos_t
            dx { x1 - x0 },
            dy { y1 - y0 };
        px_pos_t
            abs_dx { dx>=0? dx: -dx },
            abs_dy { dy>=0? dy: -dy };
        ++abs_dx;
        ++abs_dy;

        if ( abs_dx <= abs_dy ) // steep slope
        {
            // calculate how many y pixels to draw for each x pixel
            px_pos_t sct_dy { ( abs_dy + abs_dx - 1 ) / abs_dx };
            if ( sct_dy == 0 ) { ++sct_dy; }
            if ( dy < 0 ) { sct_dy = -sct_dy; }

            px_pos_t sct_y_base { y0 };
            for ( px_pos_t x { x0 }; ; )
            {
                for ( px_pos_t y { sct_y_base }; y != sct_y_base + sct_dy; )
                {
                    drawpixel( x, y, true );
                    if ( dy >= 0 ) { ++y; } else { --y; }
                }
                sct_y_base += sct_dy;
                if ( x == x1 ) { break; }
                if ( dx >= 0 ) { ++x; } else { --x; }
            }
        }
        else // shallow slope
        {
            px_pos_t sct_dx { ( abs_dx + abs_dy - 1 ) / abs_dy };
            if ( sct_dx == 0 ) { ++sct_dx; }
            if ( dx < 0 ) { sct_dx = -sct_dx; }

            px_pos_t sct_x_base { x0 };
            for ( px_pos_t y { y0 }; ; )
            {
                for ( px_pos_t x { sct_x_base }; x != sct_x_base+sct_dx; )
                {
                    drawpixel( x, y, true );
                    if ( dx >= 0 ) { ++x; } else { --x; }
                }
                sct_x_base += sct_dx;
                if ( y == y1 ) { break; }
                if ( dy>=0 ) { ++y; } else { --y; }
            }
        }
    }

}
#endif
