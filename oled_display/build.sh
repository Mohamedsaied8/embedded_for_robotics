#!/bin/bash
# Build script for OLED Display Project

set -e

BUILD_DIR="build"
BUILD_TYPE="${1:-Debug}"

echo "==================================="
echo "  OLED Display Project Builder"
echo "==================================="
echo "Build type: $BUILD_TYPE"
echo ""

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Run CMake
echo "Running CMake..."
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ..

# Build
echo ""
echo "Building..."
make -j$(nproc)

echo ""
echo "==================================="
echo "Build completed successfully!"
echo "Output files:"
ls -la *.elf *.hex *.bin 2>/dev/null || echo "  (check build directory)"
echo "==================================="
