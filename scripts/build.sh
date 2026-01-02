#!/bin/bash

# FXboard Build Script

set -e

echo "=== FXBoard Build Script ==="
echo ""

# Create build directory
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

cd build

# CMake configuration
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
echo "Building..."
cmake --build . -j$(nproc)

echo ""
echo "=== Build Complete! ==="
echo ""
echo "Run: ./FXBoard_artefacts/Release/FXBoard"
echo "Or install: cd .. && sudo scripts/install.sh"
echo ""
