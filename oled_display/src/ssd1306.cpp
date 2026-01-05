/**
  ******************************************************************************
  * @file    ssd1306.cpp
  * @brief   SSD1306 OLED display driver implementation
  ******************************************************************************
  */

#include "ssd1306.hpp"
#include "fonts.hpp"
#include <cstring>
#include <cstdlib>

SSD1306::SSD1306(I2C& i2c) : m_i2c(i2c), m_buffer{0} {
}

void SSD1306::sendCommand(uint8_t cmd) {
    uint8_t data[2] = {0x00, cmd};  // Co=0, D/C#=0 (command mode)
    m_i2c.writeData(0x00, &cmd, 1);
}

void SSD1306::sendData(const uint8_t* data, uint16_t len) {
    // Send data in chunks (I2C buffer limitation)
    const uint16_t chunkSize = 16;
    for (uint16_t i = 0; i < len; i += chunkSize) {
        uint16_t remaining = len - i;
        uint16_t toSend = (remaining < chunkSize) ? remaining : chunkSize;
        m_i2c.writeData(0x40, &data[i], toSend);
    }
}

void SSD1306::init() {
    // Wait for display to power up
    HAL_Delay(100);
    
    // Initialization sequence for 128x64 OLED
    sendCommand(SSD1306_DISPLAYOFF);          // Display OFF
    
    sendCommand(SSD1306_SETDISPLAYCLOCKDIV);  // Set clock divider
    sendCommand(0x80);                        // Suggested ratio
    
    sendCommand(SSD1306_SETMULTIPLEX);        // Set multiplex
    sendCommand(0x3F);                        // 64 MUX
    
    sendCommand(SSD1306_SETDISPLAYOFFSET);    // Set display offset
    sendCommand(0x00);                        // No offset
    
    sendCommand(SSD1306_SETSTARTLINE | 0x00); // Set start line to 0
    
    sendCommand(SSD1306_CHARGEPUMP);          // Charge pump
    sendCommand(0x14);                        // Enable charge pump
    
    sendCommand(SSD1306_MEMORYMODE);          // Memory mode
    sendCommand(0x00);                        // Horizontal addressing
    
    sendCommand(SSD1306_SEGREMAP | 0x01);     // Segment re-map
    
    sendCommand(SSD1306_COMSCANDEC);          // COM scan direction
    
    sendCommand(SSD1306_SETCOMPINS);          // COM pins
    sendCommand(0x12);                        // Alternative COM pin config
    
    sendCommand(SSD1306_SETCONTRAST);         // Set contrast
    sendCommand(0xCF);                        // Max contrast
    
    sendCommand(SSD1306_SETPRECHARGE);        // Set pre-charge period
    sendCommand(0xF1);                        // Phase 1=15, Phase 2=1
    
    sendCommand(SSD1306_SETVCOMDETECT);       // Set VCOMH deselect level
    sendCommand(0x40);                        // 0.77 x VCC
    
    sendCommand(SSD1306_DISPLAYALLON_RESUME); // Resume to RAM content
    
    sendCommand(SSD1306_NORMALDISPLAY);       // Normal display (not inverted)
    
    sendCommand(SSD1306_DISPLAYON);           // Display ON
    
    // Clear the display
    clear();
    display();
}

void SSD1306::clear(Color color) {
    uint8_t fillByte = (color == Color::White) ? 0xFF : 0x00;
    memset(m_buffer, fillByte, SSD1306_BUFFER_SIZE);
}

void SSD1306::display() {
    // Set column address range
    sendCommand(SSD1306_COLUMNADDR);
    sendCommand(0);                  // Start column
    sendCommand(SSD1306_WIDTH - 1);  // End column
    
    // Set page address range
    sendCommand(SSD1306_PAGEADDR);
    sendCommand(0);                  // Start page
    sendCommand(7);                  // End page (8 pages for 64 pixels)
    
    // Send buffer
    sendData(m_buffer, SSD1306_BUFFER_SIZE);
}

void SSD1306::drawPixel(uint8_t x, uint8_t y, Color color) {
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        return;  // Out of bounds
    }
    
    uint16_t byteIndex = x + (y / 8) * SSD1306_WIDTH;
    uint8_t bitMask = 1 << (y % 8);
    
    if (color == Color::White) {
        m_buffer[byteIndex] |= bitMask;
    } else {
        m_buffer[byteIndex] &= ~bitMask;
    }
}

void SSD1306::drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, Color color) {
    // Bresenham's line algorithm
    int16_t dx = abs(static_cast<int16_t>(x1) - static_cast<int16_t>(x0));
    int16_t dy = -abs(static_cast<int16_t>(y1) - static_cast<int16_t>(y0));
    int8_t sx = x0 < x1 ? 1 : -1;
    int8_t sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy;
    
    while (true) {
        drawPixel(x0, y0, color);
        
        if (x0 == x1 && y0 == y1) break;
        
        int16_t e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void SSD1306::drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, Color color) {
    drawLine(x, y, x + w - 1, y, color);           // Top
    drawLine(x, y + h - 1, x + w - 1, y + h - 1, color); // Bottom
    drawLine(x, y, x, y + h - 1, color);           // Left
    drawLine(x + w - 1, y, x + w - 1, y + h - 1, color); // Right
}

void SSD1306::fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, Color color) {
    for (uint8_t i = 0; i < h; i++) {
        drawLine(x, y + i, x + w - 1, y + i, color);
    }
}

uint8_t SSD1306::drawChar(uint8_t x, uint8_t y, char c, Color color) {
    const uint8_t* charData = getCharData(c);
    
    for (uint8_t col = 0; col < FONT_WIDTH; col++) {
        uint8_t colData = charData[col];
        for (uint8_t row = 0; row < FONT_HEIGHT; row++) {
            if (colData & (1 << row)) {
                drawPixel(x + col, y + row, color);
            }
        }
    }
    
    return FONT_WIDTH + 1;  // Return character width + spacing
}

void SSD1306::drawString(uint8_t x, uint8_t y, const char* str, Color color) {
    uint8_t curX = x;
    
    while (*str) {
        if (curX + FONT_WIDTH > SSD1306_WIDTH) {
            // Wrap to next line
            curX = x;
            y += FONT_HEIGHT + 1;
            
            if (y + FONT_HEIGHT > SSD1306_HEIGHT) {
                break;  // No more room
            }
        }
        
        curX += drawChar(curX, y, *str, color);
        str++;
    }
}

void SSD1306::setContrast(uint8_t contrast) {
    sendCommand(SSD1306_SETCONTRAST);
    sendCommand(contrast);
}

void SSD1306::invertDisplay(bool invert) {
    sendCommand(invert ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY);
}
