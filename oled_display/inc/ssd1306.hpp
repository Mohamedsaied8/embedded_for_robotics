/**
  ******************************************************************************
  * @file    ssd1306.hpp
  * @brief   SSD1306 OLED display driver class
  ******************************************************************************
  */

#ifndef __SSD1306_HPP
#define __SSD1306_HPP

#include "i2c.hpp"
#include <cstdint>

// Display dimensions
#define SSD1306_WIDTH   128
#define SSD1306_HEIGHT  64
#define SSD1306_BUFFER_SIZE (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

// SSD1306 Commands
#define SSD1306_SETCONTRAST         0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON        0xA5
#define SSD1306_NORMALDISPLAY       0xA6
#define SSD1306_INVERTDISPLAY       0xA7
#define SSD1306_DISPLAYOFF          0xAE
#define SSD1306_DISPLAYON           0xAF
#define SSD1306_SETDISPLAYOFFSET    0xD3
#define SSD1306_SETCOMPINS          0xDA
#define SSD1306_SETVCOMDETECT       0xDB
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5
#define SSD1306_SETPRECHARGE        0xD9
#define SSD1306_SETMULTIPLEX        0xA8
#define SSD1306_SETLOWCOLUMN        0x00
#define SSD1306_SETHIGHCOLUMN       0x10
#define SSD1306_SETSTARTLINE        0x40
#define SSD1306_MEMORYMODE          0x20
#define SSD1306_COLUMNADDR          0x21
#define SSD1306_PAGEADDR            0x22
#define SSD1306_COMSCANINC          0xC0
#define SSD1306_COMSCANDEC          0xC8
#define SSD1306_SEGREMAP            0xA0
#define SSD1306_CHARGEPUMP          0x8D

// Colors
enum class Color : uint8_t {
    Black = 0,
    White = 1
};

/**
 * @brief SSD1306 OLED Display Driver Class
 * 
 * This class provides methods to control an SSD1306 OLED display
 * via I2C interface. Supports 128x64 pixel resolution.
 */
class SSD1306 {
public:
    /**
     * @brief Construct a new SSD1306 object
     * @param i2c Reference to I2C driver object
     */
    explicit SSD1306(I2C& i2c);
    
    /**
     * @brief Initialize the display
     * Sends initialization sequence to configure the display
     */
    void init();
    
    /**
     * @brief Clear the display buffer
     * @param color Fill color (default: Black)
     */
    void clear(Color color = Color::Black);
    
    /**
     * @brief Update display with buffer contents
     * Sends the entire buffer to the display
     */
    void display();
    
    /**
     * @brief Draw a single pixel
     * @param x X coordinate (0-127)
     * @param y Y coordinate (0-63)
     * @param color Pixel color
     */
    void drawPixel(uint8_t x, uint8_t y, Color color);
    
    /**
     * @brief Draw a line between two points
     * @param x0 Start X coordinate
     * @param y0 Start Y coordinate
     * @param x1 End X coordinate
     * @param y1 End Y coordinate
     * @param color Line color
     */
    void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, Color color);
    
    /**
     * @brief Draw a rectangle
     * @param x Top-left X coordinate
     * @param y Top-left Y coordinate
     * @param w Width
     * @param h Height
     * @param color Line color
     */
    void drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, Color color);
    
    /**
     * @brief Draw a filled rectangle
     * @param x Top-left X coordinate
     * @param y Top-left Y coordinate
     * @param w Width
     * @param h Height
     * @param color Fill color
     */
    void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, Color color);
    
    /**
     * @brief Draw a character at position
     * @param x X coordinate
     * @param y Y coordinate
     * @param c Character to draw
     * @param color Text color
     * @return Width of character drawn
     */
    uint8_t drawChar(uint8_t x, uint8_t y, char c, Color color);
    
    /**
     * @brief Draw a string at position
     * @param x X coordinate
     * @param y Y coordinate
     * @param str Null-terminated string
     * @param color Text color
     */
    void drawString(uint8_t x, uint8_t y, const char* str, Color color);
    
    /**
     * @brief Set display contrast
     * @param contrast Contrast value (0-255)
     */
    void setContrast(uint8_t contrast);
    
    /**
     * @brief Invert display colors
     * @param invert true to invert, false for normal
     */
    void invertDisplay(bool invert);

private:
    I2C& m_i2c;
    uint8_t m_buffer[SSD1306_BUFFER_SIZE];
    
    /**
     * @brief Send command to display
     * @param cmd Command byte
     */
    void sendCommand(uint8_t cmd);
    
    /**
     * @brief Send data to display
     * @param data Data buffer
     * @param len Data length
     */
    void sendData(const uint8_t* data, uint16_t len);
};

#endif /* __SSD1306_HPP */
