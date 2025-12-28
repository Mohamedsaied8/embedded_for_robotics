# Quick Reference

## Build Commands

```bash
# Build project
./build.sh

# Rebuild from scratch
./build.sh rebuild

# Clean build directory
./build.sh clean

# Manual build
mkdir build && cd build
cmake ..
make
```

## Flash Commands

```bash
# Flash using st-flash (recommended)
./scripts/flash.sh

# Flash manually
st-flash --reset write build/stm32_gpio_test.bin 0x08000000

# Flash using OpenOCD
openocd -f scripts/openocd.cfg \
  -c "program build/stm32_gpio_test.elf verify reset exit"
```

## Debug Commands

```bash
# Terminal 1: Start OpenOCD server
openocd -f scripts/openocd.cfg

# Terminal 2: Connect GDB
arm-none-eabi-gdb build/stm32_gpio_test.elf
(gdb) target remote localhost:3333
(gdb) monitor reset halt
(gdb) load
(gdb) break main
(gdb) continue
```

## Utility Commands

```bash
# Check STLink connection
st-info --probe

# View binary size
arm-none-eabi-size build/stm32_gpio_test.elf

# Disassemble binary
arm-none-eabi-objdump -d build/stm32_gpio_test.elf > disassembly.txt

# View memory map
cat build/stm32_gpio_test.map
```

## GPIO Test Pins

| Pin  | Function | Description |
|------|----------|-------------|
| PC13 | Output   | Onboard LED (blinks 500ms) |
| PA5  | Output   | Test pin (toggles 500ms) |

## Memory Layout

- **Flash**: 0x08000000 - 0x0800FFFF (64 KB)
- **RAM**: 0x20000000 - 0x20004FFF (20 KB)
- **Stack**: 1 KB
- **Heap**: 512 bytes

## Clock Configuration

- **System Clock**: 72 MHz
- **AHB**: 72 MHz
- **APB1**: 36 MHz
- **APB2**: 72 MHz
