#!/bin/bash
# test_build.sh - Compilation Test Script for Project Starbeam V2
# Tests compilation without requiring physical hardware

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
SKETCH_PATH="starbeam_v2/starbeam_v2.ino"
BOARD_FQBN="esp32:esp32:esp32:PartitionScheme=huge_app"
BUILD_DIR="build_test"

echo "======================================"
echo "Project Starbeam V2 - Build Test"
echo "======================================"
echo ""

# Check if arduino-cli is installed
if ! command -v arduino-cli &> /dev/null; then
    echo -e "${RED}ERROR: arduino-cli not found${NC}"
    echo "Install from: https://arduino.github.io/arduino-cli/"
    exit 1
fi

echo -e "${GREEN}✓${NC} Arduino CLI found: $(arduino-cli version | head -1)"

# Check if ESP32 core is installed
if ! arduino-cli board listall | grep -q "esp32:esp32:esp32"; then
    echo -e "${YELLOW}Installing ESP32 core...${NC}"
    arduino-cli core update-index
    arduino-cli core install esp32:esp32
else
    echo -e "${GREEN}✓${NC} ESP32 core installed"
fi

# Check required libraries
echo ""
echo "Checking required libraries..."

REQUIRED_LIBS=(
    "Adafruit GFX Library"
    "Adafruit SSD1306"
    "U8g2"
    "RF24"
)

MISSING_LIBS=()

for lib in "${REQUIRED_LIBS[@]}"; do
    if arduino-cli lib list | grep -q "$lib"; then
        echo -e "${GREEN}✓${NC} $lib"
    else
        echo -e "${RED}✗${NC} $lib (missing)"
        MISSING_LIBS+=("$lib")
    fi
done

# Install missing libraries
if [ ${#MISSING_LIBS[@]} -gt 0 ]; then
    echo ""
    echo -e "${YELLOW}Installing missing libraries...${NC}"
    for lib in "${MISSING_LIBS[@]}"; do
        echo "Installing $lib..."
        arduino-cli lib install "$lib"
    done
fi

# Note about ELECHOUSE_CC1101
echo ""
echo -e "${YELLOW}NOTE:${NC} ELECHOUSE_CC1101_SRC_DRV library may need manual installation"
echo "  Download from: https://github.com/LSatan/SmartRC-CC1101-Driver-Lib"
echo "  In mock mode, CC1101 calls will be simulated"

# Compile the sketch
echo ""
echo "======================================"
echo "Compiling sketch..."
echo "Board: ESP32 Dev Module"
echo "Partition: Huge APP (3MB No OTA)"
echo "======================================"

# Clean build directory
rm -rf "$BUILD_DIR"

# Compile with verbose output
if arduino-cli compile \
    --fqbn "$BOARD_FQBN" \
    --build-path "$BUILD_DIR" \
    --warnings all \
    "$SKETCH_PATH" 2>&1 | tee compile.log; then

    echo ""
    echo -e "${GREEN}======================================"
    echo "✓ COMPILATION SUCCESSFUL"
    echo "======================================${NC}"
    echo ""

    # Show binary size
    if [ -f "$BUILD_DIR/starbeam_v2.ino.bin" ]; then
        SIZE=$(ls -lh "$BUILD_DIR/starbeam_v2.ino.bin" | awk '{print $5}')
        echo "Binary size: $SIZE"
    fi

    # Show memory usage from log
    echo ""
    echo "Memory Usage:"
    grep -A 5 "Sketch uses" compile.log || echo "Memory info not available"

    # Extract flash usage percentage
    echo ""
    echo "Partition Scheme: Huge APP (3MB No OTA / 1MB SPIFFS)"
    FLASH_INFO=$(grep "Sketch uses" compile.log | head -1)
    if [[ $FLASH_INFO =~ ([0-9]+)\ bytes\ \(([0-9]+)%\) ]]; then
        BYTES=${BASH_REMATCH[1]}
        PERCENT=${BASH_REMATCH[2]}
        MB=$(echo "scale=2; $BYTES / 1048576" | bc)
        echo "Flash: ${MB} MB / 3.0 MB (${PERCENT}%)"
    fi

    echo ""
    echo -e "${GREEN}Build test PASSED ✓${NC}"
    exit 0
else
    echo ""
    echo -e "${RED}======================================"
    echo "✗ COMPILATION FAILED"
    echo "======================================${NC}"
    echo ""
    echo "Check compile.log for details"
    echo ""
    echo "Common issues:"
    echo "  1. Missing libraries - install via arduino-cli lib install <name>"
    echo "  2. ESP32 core not installed - arduino-cli core install esp32:esp32"
    echo "  3. Syntax errors - check the error messages above"
    echo ""
    echo -e "${RED}Build test FAILED ✗${NC}"
    exit 1
fi
