#!/bin/bash

# Flash script for STM32F103C8T6 using st-flash with STLink v2
# Usage: ./flash.sh [path/to/binary.bin]

# Default binary path
BINARY="${1:-build/quadrature_encoder_baremetal.bin}"

# Check if binary exists
if [ ! -f "$BINARY" ]; then
    echo "Error: Binary file not found: $BINARY"
    echo "Usage: $0 [path/to/binary.bin]"
    exit 1
fi

# Flash address for STM32F103
FLASH_ADDR=0x08000000

echo "========================================="
echo "STM32 Flash Script"
echo "========================================="
echo "Binary: $BINARY"
echo "Flash Address: $FLASH_ADDR"
echo "========================================="

# Check if st-flash is installed
if ! command -v st-flash &> /dev/null; then
    echo "Error: st-flash not found!"
    echo "Please install stlink tools:"
    echo "  Ubuntu/Debian: sudo apt install stlink-tools"
    echo "  Arch: sudo pacman -S stlink"
    exit 1
fi

# Flash the binary
echo "Flashing..."
st-flash --reset write "$BINARY" "$FLASH_ADDR"

if [ $? -eq 0 ]; then
    echo "========================================="
    echo "Flash successful!"
    echo "========================================="
else
    echo "========================================="
    echo "Flash failed!"
    echo "========================================="
    exit 1
fi
