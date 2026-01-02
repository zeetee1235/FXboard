#!/bin/bash

# FXboard Install Script

set -e

if [ "$EUID" -ne 0 ]; then
    echo "Please run as root (use sudo)"
    exit 1
fi

echo "=== FXBoard Install Script ==="
echo ""

# Check if build exists
if [ ! -f "build/FXBoard_artefacts/Release/FXBoard" ] && [ ! -f "build/FXBoard_artefacts/Debug/FXBoard" ]; then
    echo "Error: FXBoard binary not found. Please build first:"
    echo "  ./scripts/build.sh"
    exit 1
fi

# Install binary
echo "Installing FXBoard binary..."
if [ -f "build/FXBoard_artefacts/Release/FXBoard" ]; then
    install -m 755 build/FXBoard_artefacts/Release/FXBoard /usr/local/bin/fxboard
else
    install -m 755 build/FXBoard_artefacts/Debug/FXBoard /usr/local/bin/fxboard
fi
echo "✓ Binary installed to /usr/local/bin/fxboard"

# Create config directory
echo "Creating config directory..."
mkdir -p /etc/fxboard
if [ ! -f /etc/fxboard/config.json ]; then
    install -m 644 config/fxboard.json.example /etc/fxboard/config.json
    echo "✓ Config installed to /etc/fxboard/config.json"
else
    echo "✓ Config already exists at /etc/fxboard/config.json"
fi

# Create samples directory
mkdir -p /usr/local/share/fxboard/samples
echo "✓ Samples directory created at /usr/local/share/fxboard/samples"

# Install udev rules
echo "Installing udev rules..."
./scripts/setup_permissions.sh

echo ""
echo "=== Installation Complete! ==="
echo ""
echo "Next steps:"
echo "  1. Add your user to input group: sudo usermod -aG input \$USER"
echo "  2. Log out and log back in"
echo "  3. Place samples in: /usr/local/share/fxboard/samples/"
echo "  4. Run: fxboard"
echo ""
