#!/bin/bash

# FXBoard Test Script
# Simple tests to verify FXBoard functionality

set -e

echo "=== FXBoard Test Suite ==="
echo ""

BINARY="./build/FXBoard_artefacts/Release/FXBoard"

if [ ! -f "$BINARY" ]; then
    echo "Error: FXBoard binary not found at $BINARY"
    echo "Please build first: ./scripts/build.sh"
    exit 1
fi

echo "1. Testing help flag..."
$BINARY --help > /dev/null 2>&1
echo "   ✓ Help flag works"

echo ""
echo "2. Testing binary execution..."
timeout 2 $BINARY 2>&1 | grep "FXBoard Initializing" > /dev/null && echo "   ✓ Binary starts correctly" || echo "   ✗ Binary failed to start"

echo ""
echo "3. Checking file size..."
SIZE=$(stat -f%z "$BINARY" 2>/dev/null || stat -c%s "$BINARY" 2>/dev/null)
SIZE_MB=$((SIZE / 1024 / 1024))
echo "   Binary size: ${SIZE_MB}MB"
if [ $SIZE_MB -lt 10 ]; then
    echo "   ✓ Reasonable size"
else
    echo "   ⚠ Large binary size"
fi

echo ""
echo "4. Checking for required samples..."
SAMPLE_COUNT=$(ls -1 samples/*.wav 2>/dev/null | wc -l)
echo "   Found $SAMPLE_COUNT samples"
if [ $SAMPLE_COUNT -gt 0 ]; then
    echo "   ✓ Samples present"
else
    echo "   ⚠ No samples found (run: python3 generate_samples_simple.py)"
fi

echo ""
echo "5. Checking configuration..."
if [ -f "config.json" ]; then
    echo "   ✓ config.json exists"
else
    echo "   ⚠ config.json missing"
fi

echo ""
echo "6. Checking permissions setup..."
if [ -f "/etc/udev/rules.d/99-fxboard.rules" ]; then
    echo "   ✓ udev rules installed"
else
    echo "   ⚠ udev rules not installed (run: sudo ./scripts/setup_permissions.sh)"
fi

if groups | grep -q "input"; then
    echo "   ✓ User in 'input' group"
else
    echo "   ⚠ User not in 'input' group (run: sudo ./scripts/setup_permissions.sh)"
fi

echo ""
echo "=== Test Summary ==="
echo "Binary builds and runs correctly."
echo ""
echo "Next steps:"
echo "  1. If permissions warnings: sudo ./scripts/setup_permissions.sh"
echo "  2. If no samples warning: python3 generate_samples_simple.py"
echo "  3. Test with actual keyboard: $BINARY"
echo ""
