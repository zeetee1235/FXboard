# Installation Guide

This guide provides detailed instructions for installing FXBoard on your system.

## Prerequisites

### Linux

FXBoard requires the following dependencies:

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    libasound2-dev \
    libfreetype6-dev \
    libx11-dev \
    libxinerama-dev \
    libxrandr-dev \
    libxcursor-dev \
    mesa-common-dev
```

**Arch Linux:**
```bash
sudo pacman -S \
    base-devel \
    cmake \
    git \
    alsa-lib \
    freetype2 \
    libx11 \
    libxinerama \
    libxrandr \
    libxcursor \
    mesa
```

**Fedora:**
```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    git \
    alsa-lib-devel \
    freetype-devel \
    libX11-devel \
    libXinerama-devel \
    libXrandr-devel \
    libXcursor-devel \
    mesa-libGL-devel
```

## Building from Source

### 1. Clone the Repository

```bash
git clone https://github.com/zeetee1235/FXboard.git
cd FXboard
```

### 2. Build

Use the provided build script:

```bash
./scripts/build.sh
```

Or manually:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

The build process will:
- Download and configure JUCE framework automatically
- Compile FXBoard
- Create the executable in `build/FXBoard_artefacts/Release/FXBoard`

### 3. Setup Permissions

**This step is critical for FXBoard to access keyboard input!**

```bash
sudo ./scripts/setup_permissions.sh
```

This script will:
- Install udev rules to allow access to `/dev/input/event*` devices
- Add your user to the `input` group

**Important:** After running this script, you must:
1. Log out of your current session
2. Log back in
3. Verify group membership: `groups | grep input`

### 4. Install (Optional)

To install FXBoard system-wide:

```bash
sudo ./scripts/install.sh
```

This will:
- Install binary to `/usr/local/bin/fxboard`
- Create config directory at `/etc/fxboard/`
- Create samples directory at `/usr/local/share/fxboard/samples/`
- Install udev rules

## Permission Setup Details

### Why Permissions Are Needed

FXBoard needs to read from `/dev/input/event*` devices to capture keyboard input at a low level. By default, these devices are only accessible by root.

### What the Setup Script Does

1. **Installs udev rules** (`/etc/udev/rules.d/99-fxboard.rules`):
   - Sets `/dev/input/event*` devices to be readable by the `input` group
   
2. **Adds user to input group**:
   - Allows your user to read from input devices without sudo

### Manual Permission Setup

If you prefer to set up permissions manually:

```bash
# Install udev rules
sudo cp config/99-fxboard.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
sudo udevadm trigger

# Add user to input group
sudo usermod -aG input $USER

# Log out and log back in
```

### Verifying Permissions

After logging out and back in:

```bash
# Check group membership
groups | grep input

# Check device permissions
ls -la /dev/input/event*

# Should show something like:
# crw-rw---- 1 root input ... /dev/input/event0
```

## Sample Files

### Generate Test Samples

FXBoard includes a script to generate test samples:

```bash
python3 generate_samples_simple.py
```

This creates basic drum samples in the `samples/` directory.

### Use Your Own Samples

Copy your WAV files to the samples directory:

```bash
mkdir -p samples
cp /path/to/your/*.wav samples/
```

**Requirements:**
- Format: WAV
- Recommended: 44.1kHz or 48kHz sample rate
- Mono or stereo

## Configuration

### Default Config Locations

FXBoard looks for configuration in these locations (in order):
1. `./config.json` (current directory)
2. `./config/fxboard.json.example`
3. `/etc/fxboard/config.json` (after system install)

### Create Your Config

```bash
cp config/fxboard.json.example config.json
# Edit config.json with your preferred settings
```

See [CONFIG.md](CONFIG.md) for detailed configuration options.

## Running FXBoard

### From Build Directory

```bash
./build/FXBoard_artefacts/Release/FXBoard
```

### After System Install

```bash
fxboard
```

### With Custom Config

```bash
fxboard -c /path/to/your/config.json
```

## Troubleshooting

### Build Errors

**"Could not find JUCE"**
- JUCE is downloaded automatically. Check your internet connection.
- Try removing `build/` directory and rebuilding.

**Missing dependencies**
- Install all prerequisites listed above for your distribution.

### Permission Errors

**"Failed to open /dev/input directory"**
- Run permission setup script: `sudo ./scripts/setup_permissions.sh`
- Log out and log back in
- Verify with: `groups | grep input`

**"Permission denied for: /dev/input/eventX"**
- Check device permissions: `ls -la /dev/input/event*`
- Verify udev rules: `cat /etc/udev/rules.d/99-fxboard.rules`
- Reload udev: `sudo udevadm control --reload-rules && sudo udevadm trigger`

### Audio Errors

**"Failed to initialize audio device"**
- Check audio system is running: `pactl info` (for PulseAudio)
- Try increasing buffer size in config.json
- Check output: `aplay -l`

**No sound output**
- Verify samples are loaded (check startup messages)
- Ensure sample files exist in samples directory
- Check key mappings in config.json

### Runtime Errors

**No response to keyboard input**
- Check if keyboard device was detected (see startup messages)
- Try unplugging and replugging USB keyboard
- Run with verbose logging to see which device is used

## Uninstallation

### Remove System Install

```bash
sudo rm /usr/local/bin/fxboard
sudo rm -rf /etc/fxboard
sudo rm -rf /usr/local/share/fxboard
sudo rm /etc/udev/rules.d/99-fxboard.rules
sudo udevadm control --reload-rules
```

### Remove from input Group

```bash
sudo gpasswd -d $USER input
# Log out and back in
```

## Next Steps

- [Configuration Guide](CONFIG.md) - Customize key mappings and audio settings
- [Development Guide](DEVELOPMENT.md) - Architecture and contribution info
