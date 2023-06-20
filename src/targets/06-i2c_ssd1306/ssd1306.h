#ifndef _SSD1306_H
#define _SSD1306_H

// #include "i2c/i2c.h"

#define SSD1306_ADDR (0x3c)

typedef enum {
    MEMORYMODE = 0x20, ///< See datasheet
    COLUMNADDR = 0x21, ///< See datasheet
    PAGEADDR = 0x22, ///< See datasheet
    SETCONTRAST = 0x81, ///< See datasheet
    CHARGEPUMP = 0x8D, ///< See datasheet
    SEGREMAP = 0xA0, ///< See datasheet
    DISPLAYALLON_RESUME = 0xA4, ///< See datasheet
    DISPLAYALLON = 0xA5, ///< Not currently used
    NORMALDISPLAY = 0xA6, ///< See datasheet
    INVERTDISPLAY = 0xA7, ///< See datasheet
    SETMULTIPLEX = 0xA8, ///< See datasheet
    DISPLAYOFF = 0xAE, ///< See datasheet
    DISPLAYON = 0xAF, ///< See datasheet
    COMSCANINC = 0xC0, ///< Not currently used
    COMSCANDEC = 0xC8, ///< See datasheet
    SETDISPLAYOFFSET = 0xD3, ///< See datasheet
    SETDISPLAYCLOCKDIV = 0xD5, ///< See datasheet
    SETPRECHARGE = 0xD9, ///< See datasheet
    SETCOMPINS = 0xDA, ///< See datasheet
    SETVCOMDETECT = 0xDB, ///< See datasheet
    SETLOWCOLUMN = 0x00, ///< Not currently used
    SETHIGHCOLUMN = 0x10, ///< Not currently used
    SETSTARTLINE = 0x40, ///< See datasheet
    EXTERNALVCC = 0x01, ///< External display voltage source
    SWITCHCAPVCC = 0x02, ///< Gen. display voltage from 3.3V
    RIGHT_HORIZONTAL_SCROLL = 0x26, ///< Init rt scroll
    LEFT_HORIZONTAL_SCROLL = 0x27, ///< Init left scroll
    VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL = 0x29, ///< Init diag scroll
    VERTICAL_AND_LEFT_HORIZONTAL_SCROLL = 0x2A, ///< Init diag scroll
    DEACTIVATE_SCROLL = 0x2E, ///< Stop scroll
    ACTIVATE_SCROLL = 0x2F, ///< Start scroll
    SET_VERTICAL_SCROLL_AREA = 0xA3, ///< Set scroll range

    // CHARGEPUMP = 0x8D,
    // COLUMNADDR = 0x21,
    // COMSCANDEC = 0xC8,
    // COMSCANINC = 0xC0,
    // DISPLAYALLON = 0xA5,
    // DISPLAYALLON_RESUME = 0xA4,
    // DISPLAYOFF = 0xAE,
    // DISPLAYON = 0xAF,
    // EXTERNALVCC = 0x1,
    // INVERTDISPLAY = 0xA7,
    // MEMORYMODE = 0x20,
    // NORMALDISPLAY = 0xA6,
    // PAGEADDR = 0x22,
    // PAGESTARTADDRESS = 0xB0,
    // SEGREMAP = 0xA1,
    // SETCOMPINS = 0xDA,
    // SETCONTRAST = 0x81,
    // SETDISPLAYCLOCKDIV = 0xD5,
    // SETDISPLAYOFFSET = 0xD3,
    // SETHIGHCOLUMN = 0x10,
    // SETLOWCOLUMN = 0x00,
    // SETMULTIPLEX = 0xA8,
    // SETPRECHARGE = 0xD9,
    // SETSEGMENTREMAP = 0xA1,
    // SETSTARTLINE = 0x40,
    // SETVCOMDETECT = 0xDB,
    // SWITCHCAPVCC = 0x2,
} ssd1306_command_t;

void ssd1306(void);

#endif //_SSD1306_H
