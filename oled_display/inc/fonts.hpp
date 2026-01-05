/**
  ******************************************************************************
  * @file    fonts.hpp
  * @brief   Font definitions for OLED display
  ******************************************************************************
  */

#ifndef __FONTS_HPP
#define __FONTS_HPP

#include <cstdint>

// Font dimensions
#define FONT_WIDTH  5
#define FONT_HEIGHT 7

/**
 * @brief Get font data for a character
 * @param c ASCII character (32-126)
 * @return Pointer to font data (5 bytes per character)
 */
const uint8_t* getCharData(char c);

#endif /* __FONTS_HPP */
