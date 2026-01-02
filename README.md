# FXBoard

Ultra-low-latency keyboard sound program for rhythm games.

## Overview

FXBoard is a lightweight C++ audio tool that plays sounds in response to keyboard input with minimal latency (target: < 10ms). Designed to run alongside rhythm games without interfering with gameplay.

## Key Features

- **Ultra-low latency**: < 10ms from key press to audio output
- **Independent operation**: Works with any rhythm game
- **Lock-free architecture**: Stable real-time audio processing
- **CLI/Daemon mode**: Runs in background with minimal resource usage
- **Flexible configuration**: JSON-based key mapping and sample configuration
- **Real-time effects**: Filter, bit crusher, reverb (optional)

## Quick Start

### Prerequisites

**Linux:**
```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake libasound2-dev \
    libfreetype6-dev libx11-dev libxinerama-dev

# Arch Linux
sudo pacman -S base-devel cmake alsa-lib freetype2 libx11
```

### Installation

```bash
# Clone repository
git clone https://github.com/zeetee1235/FXboard.git
cd FXboard

# Build
./scripts/build.sh

# Setup permissions (required for keyboard access)
sudo ./scripts/setup_permissions.sh

# Log out and log back in for group changes to take effect

# Run
./build/FXBoard_artefacts/Release/FXBoard

# Or install system-wide
sudo ./scripts/install.sh
fxboard
```

### Adding Samples

Place your `.wav` audio samples in the `samples/` directory:

```bash
# Generate test samples
python3 generate_samples_simple.py

# Or copy your own samples
cp /path/to/your/kick.wav samples/
cp /path/to/your/snare.wav samples/
```

## Configuration

Edit `config.json` to customize key mappings and audio settings:

```json
{
  "audio": {
    "sampleRate": 48000,
    "bufferSize": 128,
    "outputChannels": 2
  },
  "keymapping": {
    "30": "kick",
    "31": "snare",
    "32": "hihat",
    "33": "clap"
  },
  "samples": {
    "kick": {
      "file": "samples/kick.wav",
      "gain": 1.0
    }
  }
}
```

**Note**: Key codes are Linux evdev scancodes (30=A, 31=S, 32=D, 33=F, etc.)

## Usage

```bash
# Run with default config
./FXBoard

# Use custom config
./FXBoard -c /path/to/config.json

# Show help
./FXBoard --help
```

Press `Ctrl+C` to quit gracefully.

## Project Structure

```
FXBoard/
├── src/
│   ├── main.cpp           # Entry point with signal handlers
│   ├── core/
│   │   ├── Application.h/cpp    # Main application logic
│   │   ├── EventQueue.h         # Lock-free event queue
│   │   ├── KeyEvent.h           # Key event structure
│   │   └── Smoother.h           # Parameter smoothing
│   ├── input/
│   │   ├── KeyHook.h/cpp        # Keyboard input (evdev)
│   │   └── KeyState.h           # Key state tracking
│   ├── audio/
│   │   ├── AudioEngine.h/cpp    # Audio engine
│   │   ├── SampleManager.h/cpp  # Sample loading/management
│   │   ├── Mixer.h/cpp          # Audio mixer
│   │   └── FX.h/cpp             # Effects processing
│   └── app/
│       └── ConfigManager.h/cpp  # Configuration management
├── config/
│   ├── fxboard.json.example     # Example configuration
│   └── 99-fxboard.rules         # udev rules for permissions
├── scripts/
│   ├── build.sh                 # Build script
│   ├── install.sh               # Installation script
│   └── setup_permissions.sh     # Permission setup
└── docs/                        # Detailed documentation
```

## Performance Goals

- **Latency**: < 10ms (key press to audio output)
- **CPU Usage**: < 5% in background
- **Memory**: < 50MB (including loaded samples)

## Platform Support

- **Linux**: Full support (Ubuntu 20.04+, Arch, Fedora)
- **Windows**: Planned
- **macOS**: Planned

## Documentation

- [Installation Guide](docs/INSTALL.md) - Detailed installation instructions
- [Configuration Guide](docs/CONFIG.md) - Configuration file format and options
- [Development Guide](docs/DEVELOPMENT.md) - Architecture and contribution guidelines

## Troubleshooting

### "Permission denied" errors

Make sure you've setup permissions correctly:
```bash
sudo ./scripts/setup_permissions.sh
# Log out and log back in
groups | grep input  # Verify you're in input group
```

### "Failed to find keyboard device"

Check if `/dev/input/event*` devices exist:
```bash
ls -la /dev/input/event*
```

### No sound output

1. Check that samples are loaded (look for "Loaded: sample_name" messages)
2. Verify samples directory exists with `.wav` files
3. Check audio device with `aplay -l`

## Contributing

Contributions are welcome! See [DEVELOPMENT.md](docs/DEVELOPMENT.md) for architecture details and contribution guidelines.

## License

[License information to be added]

## Acknowledgments

Built with:
- [JUCE](https://juce.com/) - Audio framework
- Linux evdev - Low-level keyboard input
