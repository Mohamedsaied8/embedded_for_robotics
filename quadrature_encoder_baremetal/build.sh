#!/bin/bash

# Build script for STM32 CMake project
# Usage: ./build.sh [clean|rebuild]

set -e  # Exit on error

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=========================================${NC}"
echo -e "${GREEN}STM32 CMake Build Script${NC}"
echo -e "${GREEN}=========================================${NC}"

# Handle arguments
if [ "$1" == "clean" ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf "$BUILD_DIR"
    echo -e "${GREEN}Clean complete!${NC}"
    exit 0
elif [ "$1" == "rebuild" ]; then
    echo -e "${YELLOW}Rebuilding (clean + build)...${NC}"
    rm -rf "$BUILD_DIR"
fi

# Create build directory
if [ ! -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Creating build directory...${NC}"
    mkdir -p "$BUILD_DIR"
fi

# Navigate to build directory
cd "$BUILD_DIR"

# Run CMake
echo -e "${YELLOW}Running CMake...${NC}"
cmake ..

# Build
echo -e "${YELLOW}Building project...${NC}"
make -j$(nproc)

# Check if build was successful
if [ $? -eq 0 ]; then
    echo -e "${GREEN}=========================================${NC}"
    echo -e "${GREEN}Build successful!${NC}"
    echo -e "${GREEN}=========================================${NC}"
    echo ""
    echo -e "Generated files in ${YELLOW}${BUILD_DIR}${NC}:"
    ls -lh *.elf *.bin *.hex 2>/dev/null || true
    echo ""
    echo -e "To flash: ${YELLOW}./scripts/flash.sh${NC}"
else
    echo -e "${RED}=========================================${NC}"
    echo -e "${RED}Build failed!${NC}"
    echo -e "${RED}=========================================${NC}"
    exit 1
fi
