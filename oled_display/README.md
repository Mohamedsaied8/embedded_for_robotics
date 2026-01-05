# OLED Display Project for STM32F103

A bare-metal C++ project for driving an SSD1306 OLED display using an Object-Oriented approach on STM32F103.

## Features

- **OOP Design**: Clean C++ classes for I2C and SSD1306 display
- **128x64 OLED Support**: Full resolution SSD1306 display driver
- **Graphics Primitives**: Pixels, lines, rectangles (outline and filled)
- **Text Rendering**: 5x7 ASCII font for printable characters
- **CMake Build System**: Cross-compilation with arm-none-eabi-gcc

## Hardware Setup

| OLED Pin | STM32F103 Pin |
|----------|---------------|
| VCC      | 3.3V          |
| GND      | GND           |
| SCL      | PB6           |
| SDA      | PB7           |

**Note**: The SSD1306 I2C address is set to 0x3C (default). Modify `OLED_I2C_ADDR` in `main.h` if your display uses 0x3D.

## Building

```bash
# Make build script executable
chmod +x build.sh

# Build in Debug mode
./build.sh

# Build in Release mode
./build.sh Release
```

## Flashing

```bash
# Using ST-Link
st-flash write build/oled_display.bin 0x08000000

# Or using OpenOCD
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
    -c "program build/oled_display.elf verify reset exit"
```

## Project Structure

```
oled_display/
├── CMakeLists.txt          # CMake build configuration
├── build.sh                # Build script
├── inc/
│   ├── main.h              # Main header with pin definitions
│   ├── i2c.hpp             # I2C driver class
│   ├── ssd1306.hpp         # SSD1306 OLED driver class
│   └── fonts.hpp           # Font definitions
└── src/
    ├── main.cpp            # Main application
    ├── i2c.cpp             # I2C implementation
    ├── ssd1306.cpp         # SSD1306 implementation
    └── fonts.cpp           # Font data
```

## API Usage

```cpp
#include "i2c.hpp"
#include "ssd1306.hpp"

// Initialize
I2C i2c(0x3C);
i2c.init();

SSD1306 display(i2c);
display.init();

// Draw
display.clear();
display.drawString(10, 10, "Hello World!", Color::White);
display.drawRect(0, 0, 128, 64, Color::White);
display.display();  // Update screen
```

## License

MIT License
