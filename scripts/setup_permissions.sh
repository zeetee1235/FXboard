#!/bin/bash

# FXboard Permission Setup Script
# This script sets up the necessary permissions for FXboard to access keyboard input devices

set -e

echo "=== FXboard Permission Setup ==="
echo ""

# Check if running as root for udev rules installation
if [ "$EUID" -eq 0 ]; then
    echo "Installing udev rules..."
    cp config/99-fxboard.rules /etc/udev/rules.d/
    udevadm control --reload-rules
    udevadm trigger
    echo "✓ udev rules installed"
    echo ""
fi

# Add current user to input group
CURRENT_USER=${SUDO_USER:-$USER}

if groups $CURRENT_USER | grep -q "\binput\b"; then
    echo "✓ User '$CURRENT_USER' is already in 'input' group"
else
    echo "Adding user '$CURRENT_USER' to 'input' group..."
    if [ "$EUID" -eq 0 ]; then
        usermod -aG input $CURRENT_USER
        echo "✓ User added to 'input' group"
        echo ""
        echo "⚠ IMPORTANT: You must log out and log back in for the group change to take effect!"
    else
        echo "⚠ Please run this script with sudo to add user to 'input' group:"
        echo "  sudo $0"
    fi
fi

echo ""
echo "=== Setup Complete ==="
echo ""
echo "If you just added the user to the 'input' group, remember to:"
echo "  1. Log out of your current session"
echo "  2. Log back in"
echo "  3. Verify with: groups | grep input"
echo ""
