# STM32 CMake Project Template

A complete CMake-based project template for STM32F103C8T6 (Blue Pill) with GPIO driver test.

## Features

- ✅ CMake build system
- ✅ STM32 HAL driver integration
- ✅ GPIO test application (blinks onboard LED on PC13 and PA5)
- ✅ STLink v2 support with st-flash
- ✅ OpenOCD configuration included
- ✅ Proper linker script for STM32F103C8T6 (64KB Flash, 20KB RAM)
- ✅ ARM GCC toolchain configuration

## Hardware Requirements

- **STM32F103C8T6** (Blue Pill board)
- **STLink v2** programmer/debugger
- **LED** (optional - for PA5 test, onboard LED on PC13 will blink)

## Software Requirements

### Required Tools

```bash
# Ubuntu/Debian
sudo apt install cmake gcc-arm-none-eabi stlink-tools openocd

# Arch Linux
sudo pacman -S cmake arm-none-eabi-gcc arm-none-eabi-newlib stlink openocd
```

### Verify Installation

```bash
arm-none-eabi-gcc --version
st-flash --version
cmake --version
```

## VSCode GUI Debugging (Recommended)

For the best development experience with GUI debugging, breakpoints, and peripheral inspection:

### Install VSCode Extensions

When you open this project in VSCode, install the recommended extensions:
- **Cortex-Debug** - Essential for STM32 GUI debugging
- **C/C++ Extension Pack** - IntelliSense and code navigation
- **CMake Tools** - CMake integration

### Quick Start

1. **Build**: Press `Ctrl+Shift+B` or run `./build.sh`
2. **Debug**: Press `F5` or click the Run/Debug button (▶️)
3. **Set Breakpoints**: Click in the left margin next to line numbers

See [VSCODE_DEBUG_GUIDE.md](VSCODE_DEBUG_GUIDE.md) for complete instructions.

### Debug Configurations Available

- **Cortex Debug (OpenOCD)** - Full GUI with peripheral registers (recommended)
- **Cortex Debug (ST-Link)** - Direct ST-Link connection
- **Debug (GDB - OpenOCD)** - Fallback GDB option

## Project Structure

```
stm32_cmake_template/
├── CMakeLists.txt              # Main CMake configuration
├── cmake/
│   └── arm-none-eabi-gcc.cmake # ARM GCC toolchain file
├── inc/                        # Header files
│   ├── main.h
│   ├── stm32f1xx_hal_conf.h
│   └── stm32f1xx_it.h
├── src/                        # Source files
│   ├── main.c                  # Main application with GPIO test
│   ├── stm32f1xx_hal_msp.c     # HAL MSP initialization
│   ├── stm32f1xx_it.c          # Interrupt handlers
│   └── system_stm32f1xx.c      # System initialization
├── startup/
│   └── startup_stm32f103xb.s   # Startup assembly file
├── linker/
│   └── STM32F103C8Tx_FLASH.ld  # Linker script
└── scripts/
    ├── flash.sh                # Flash script for st-flash
    └── openocd.cfg             # OpenOCD configuration
```

## Building the Project

### 1. Create Build Directory

```bash
cd stm32_cmake_template
mkdir build && cd build
```

### 2. Configure with CMake

```bash
cmake ..
```

### 3. Build

```bash
make
```

This will generate:
- `stm32_gpio_test.elf` - ELF executable
- `stm32_gpio_test.hex` - Intel HEX format
- `stm32_gpio_test.bin` - Binary format (for st-flash)
- `stm32_gpio_test.map` - Memory map file

## Flashing the Firmware

### Method 1: Using st-flash (Recommended)

```bash
# From project root
./scripts/flash.sh

# Or manually
st-flash --reset write build/stm32_gpio_test.bin 0x08000000
```

### Method 2: Using OpenOCD

```bash
openocd -f scripts/openocd.cfg -c "program build/stm32_gpio_test.elf verify reset exit"
```

## GPIO Test Application

The test application demonstrates basic GPIO functionality:

### Pin Configuration

| Pin  | Function | Description |
|------|----------|-------------|
| PC13 | Output   | Onboard LED (active low) - blinks every 500ms |
| PA5  | Output   | External test pin - toggles every 500ms |

### Expected Behavior

1. **PC13 (Onboard LED)**: Blinks at 500ms intervals
2. **PA5**: Toggles at 500ms intervals (can connect external LED with resistor)

### Wiring for PA5 Test (Optional)

```
PA5 ----[220Ω]----[LED]----GND
```

## Customization

### Changing Target MCU

1. Update `MCU_MODEL` in `CMakeLists.txt`
2. Replace linker script in `linker/` directory
3. Update startup file in `startup/` directory

### Adding More HAL Modules

Edit `inc/stm32f1xx_hal_conf.h` and enable required modules:

```c
#define HAL_UART_MODULE_ENABLED
#define HAL_SPI_MODULE_ENABLED
// etc.
```

Then add corresponding source files in `CMakeLists.txt`:

```cmake
file(GLOB HAL_SOURCES
    # ... existing files ...
    ${HAL_DIR}/Src/stm32f1xx_hal_uart.c
    ${HAL_DIR}/Src/stm32f1xx_hal_spi.c
)
```

## Debugging

### Using OpenOCD + GDB

Terminal 1:
```bash
openocd -f scripts/openocd.cfg
```

Terminal 2:
```bash
arm-none-eabi-gdb build/stm32_gpio_test.elf
(gdb) target remote localhost:3333
(gdb) monitor reset halt
(gdb) load
(gdb) continue
```

## Troubleshooting

### Build Errors

**Error: `arm-none-eabi-gcc not found`**
- Install ARM GCC toolchain (see Software Requirements)

**Error: `Cannot find HAL files`**
- Ensure `STM32Cube_FW_F1_V1.8.0` is in the parent directory
- Check `CUBE_ROOT` path in `CMakeLists.txt`

### Flash Errors

**Error: `st-flash: error while loading shared libraries`**
```bash
sudo apt install libusb-1.0-0-dev
```

**Error: `Failed to connect to target`**
- Check STLink v2 connection
- Verify USB cable
- Try: `st-info --probe`

**Error: `Permission denied`**
```bash
# Add udev rules for STLink
sudo cp /usr/share/stlink/49-stlinkv*.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
sudo udevadm trigger
# Replug STLink
```

## Clock Configuration

- **HSE**: 8 MHz external crystal
- **PLL**: 9x multiplier
- **SYSCLK**: 72 MHz (maximum for STM32F103)
- **AHB**: 72 MHz
- **APB1**: 36 MHz
- **APB2**: 72 MHz

## Memory Layout

- **Flash**: 64 KB (0x08000000 - 0x0800FFFF)
- **RAM**: 20 KB (0x20000000 - 0x20004FFF)
- **Stack**: 1 KB
- **Heap**: 512 bytes

## License

This template is provided as-is for educational and commercial use.

## References

- [STM32F103 Reference Manual](https://www.st.com/resource/en/reference_manual/cd00171190.pdf)
- [STM32F103 Datasheet](https://www.st.com/resource/en/datasheet/stm32f103c8.pdf)
- [STM32CubeF1 HAL Documentation](https://www.st.com/en/embedded-software/stm32cubef1.html)

## Contributing

Feel free to submit issues and enhancement requests!
