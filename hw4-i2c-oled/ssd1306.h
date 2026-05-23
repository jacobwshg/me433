#ifndef SSD1306_H__
#define SSD1306_H__

#include <cstdint>

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

// namespace SSD1306
// {
//     enum class Command: std::uint8_t
//     {
//         MEMORYMODE          = 0x20,
//         COLUMNADDR          = 0x21,
//         PAGEADDR            = 0x22,
//         SETCONTRAST         = 0x81,
//         CHARGEPUMP          = 0x8D,
//         SEGREMAP            = 0xA0,
//         DISPLAYALLON_RESUME = 0xA4,
//         NORMALDISPLAY       = 0xA6,
//         INVERTDISPLAY       = 0xA7,
//         SETMULTIPLEX        = 0xA8,
//         DISPLAYOFF          = 0xAE,
//         DISPLAYON           = 0xAF,
//         COMSCANDEC          = 0xC8,
//         SETDISPLAYOFFSET    = 0xD3,
//         SETDISPLAYCLOCKDIV  = 0xD5,
//         SETPRECHARGE        = 0xD9,
//         SETCOMPINS          = 0xDA,
//         SETVCOMDETECT       = 0xDB,
//         SETSTARTLINE        = 0x40,
//         DEACTIVATE_SCROLL   = 0x2E,
//     };
// }

void ssd1306_setup(void);
void ssd1306_update(void);
void ssd1306_clear(void);
void ssd1306_drawPixel(unsigned char x, unsigned char y, unsigned char color);

/// this should be private
//void ssd1306_command(unsigned char c);

#endif
