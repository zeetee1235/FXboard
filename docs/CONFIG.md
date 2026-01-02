# Configuration Guide

This guide explains how to configure FXBoard through the JSON configuration file.

## Configuration File Location

FXBoard searches for configuration files in the following order:

1. Path specified with `-c` or `--config` flag
2. `./config.json` (current directory)
3. `./config/fxboard.json.example`
4. `/etc/fxboard/config.json` (system-wide install)

## Configuration File Format

The configuration file is in JSON format with the following main sections:

```json
{
  "audio": { ... },
  "keymapping": { ... },
  "samples": { ... },
  "fx": { ... }
}
```

## Audio Configuration

Configure audio device settings:

```json
{
  "audio": {
    "sampleRate": 48000,
    "bufferSize": 128,
    "outputChannels": 2,
    "deviceName": ""
  }
}
```

### Options

- **sampleRate** (number): Sample rate in Hz
  - Common values: `44100`, `48000`, `96000`
  - Higher rates = better quality but more CPU usage
  - Default: `48000`

- **bufferSize** (number): Audio buffer size in samples
  - Lower values = lower latency but more CPU usage
  - Recommended for low latency: `64`, `128`, `256`
  - If you hear crackling, increase this value
  - Default: `128`

- **outputChannels** (number): Number of output channels
  - `2` = stereo
  - `1` = mono
  - Default: `2`

- **deviceName** (string): Specific audio device name
  - Leave empty (`""`) to use default device
  - To see available devices, check system audio settings

### Latency Calculation

Total latency = (bufferSize / sampleRate) * 1000 ms

Examples:
- 128 samples @ 48kHz = 2.67ms
- 256 samples @ 48kHz = 5.33ms
- 64 samples @ 48kHz = 1.33ms

## Key Mapping

Map keyboard keys (by scancode) to sample IDs:

```json
{
  "keymapping": {
    "30": "kick",
    "31": "snare",
    "32": "hihat",
    "33": "clap",
    "36": "kick",
    "37": "snare",
    "38": "hihat",
    "39": "clap"
  }
}
```

### Linux Evdev Scancodes

Common keys and their scancodes:

| Key | Scancode | Key | Scancode |
|-----|----------|-----|----------|
| A   | 30       | J   | 36       |
| S   | 31       | K   | 37       |
| D   | 32       | L   | 38       |
| F   | 33       | ;   | 39       |
| Q   | 16       | U   | 22       |
| W   | 17       | I   | 23       |
| E   | 18       | O   | 24       |
| R   | 19       | P   | 25       |
| Space | 57     | Enter | 28     |
| Left Alt | 56 | Right Alt | 100 |
| Left Ctrl | 29 | Right Ctrl | 97 |

### Finding Scancodes

To find scancodes for specific keys:

```bash
# Install evtest
sudo apt-get install evtest

# Run evtest and press keys
sudo evtest
```

The "code" value shown is the scancode to use in your config.

## Sample Configuration

Define samples and their properties:

```json
{
  "samples": {
    "kick": {
      "file": "samples/kick.wav",
      "gain": 1.0
    },
    "snare": {
      "file": "samples/snare.wav",
      "gain": 0.9
    },
    "hihat": {
      "file": "samples/hihat.wav",
      "gain": 0.8
    },
    "clap": {
      "file": "samples/clap.wav",
      "gain": 0.85
    }
  }
}
```

### Sample Options

- **file** (string): Path to WAV file (relative or absolute)
- **gain** (number): Volume multiplier (0.0 to 1.0)
  - `1.0` = full volume
  - `0.5` = half volume
  - `0.0` = muted

### Sample File Requirements

- **Format**: WAV (other formats may be added later)
- **Sample Rate**: Any (automatically resampled)
- **Bit Depth**: 16-bit or 24-bit recommended
- **Channels**: Mono or stereo

## Effects Configuration

Configure audio effects (experimental):

```json
{
  "fx": {
    "filter": {
      "enabled": false,
      "cutoff": 1000,
      "resonance": 0.707
    },
    "bitcrusher": {
      "enabled": false,
      "bitDepth": 16,
      "downsample": 1
    },
    "reverb": {
      "enabled": false,
      "mix": 0.0,
      "decay": 0.5
    }
  }
}
```

### Filter

Low-pass filter effect:

- **enabled** (boolean): Enable/disable filter
- **cutoff** (number): Cutoff frequency in Hz (20-20000)
  - Lower = darker sound
- **resonance** (number): Filter resonance (0.1-10.0)
  - Higher = more pronounced peak at cutoff

### Bitcrusher

Digital distortion effect:

- **enabled** (boolean): Enable/disable bitcrusher
- **bitDepth** (number): Bit depth (1-16)
  - Lower = more distortion
- **downsample** (number): Sample rate reduction (1-32)
  - Higher = more aliasing/distortion

### Reverb

Reverberation effect:

- **enabled** (boolean): Enable/disable reverb
- **mix** (number): Wet/dry mix (0.0-1.0)
  - `0.0` = completely dry
  - `1.0` = completely wet
- **decay** (number): Decay time (0.0-1.0)
  - Higher = longer reverb tail

## Example Configurations

### Minimal Configuration

For basic key-to-sample mapping:

```json
{
  "audio": {
    "bufferSize": 128
  },
  "keymapping": {
    "30": "kick",
    "31": "snare"
  },
  "samples": {
    "kick": {"file": "samples/kick.wav", "gain": 1.0},
    "snare": {"file": "samples/snare.wav", "gain": 1.0}
  }
}
```

### Low Latency Configuration

Optimized for minimum latency:

```json
{
  "audio": {
    "sampleRate": 48000,
    "bufferSize": 64,
    "outputChannels": 2
  },
  "keymapping": {
    "30": "kick"
  },
  "samples": {
    "kick": {"file": "samples/kick.wav", "gain": 1.0}
  }
}
```

Note: Buffer size of 64 may cause audio glitches on some systems. Increase to 128 or 256 if needed.

### 10-Key Rhythm Game Setup

For games like Sound Voltex or similar:

```json
{
  "audio": {
    "sampleRate": 48000,
    "bufferSize": 128
  },
  "keymapping": {
    "30": "bt_a",
    "31": "bt_b",
    "32": "bt_c",
    "33": "bt_d",
    "36": "fx_l",
    "38": "fx_r",
    "56": "bt_a",
    "57": "bt_b"
  },
  "samples": {
    "bt_a": {"file": "samples/bt_a.wav", "gain": 1.0},
    "bt_b": {"file": "samples/bt_b.wav", "gain": 1.0},
    "bt_c": {"file": "samples/bt_c.wav", "gain": 1.0},
    "bt_d": {"file": "samples/bt_d.wav", "gain": 1.0},
    "fx_l": {"file": "samples/fx_l.wav", "gain": 0.8},
    "fx_r": {"file": "samples/fx_r.wav", "gain": 0.8}
  }
}
```

## Reloading Configuration

Currently, FXBoard must be restarted to reload configuration changes:

1. Stop FXBoard (Ctrl+C)
2. Edit config file
3. Restart FXBoard

Hot-reloading may be added in a future version.

## Troubleshooting

### Configuration not loading

- Check JSON syntax with a validator
- Ensure file path is correct
- Check file permissions (must be readable)
- Look for error messages at startup

### Samples not playing

- Verify sample file paths exist
- Check file format (must be WAV)
- Ensure gain is not 0.0
- Check that keys are mapped to sample IDs

### Audio glitches/crackling

- Increase `bufferSize` (try 256 or 512)
- Close other audio applications
- Check system CPU usage
- Disable effects if enabled

### Wrong keys triggering

- Use `evtest` to find correct scancodes
- Update keymapping section
- Restart FXBoard after config changes

## Advanced Tips

### Profile Management

Create multiple config files for different games:

```bash
# Create profiles
cp config.json config.voltex.json
cp config.json config.beatmania.json

# Switch profiles
fxboard -c config.voltex.json
```

### Gain Staging

To prevent clipping:
- Keep individual sample gains ≤ 0.8
- Test with multiple keys pressed simultaneously
- If clipping occurs, reduce all gains proportionally

### Performance Tuning

For best performance:
1. Use smallest comfortable buffer size
2. Disable effects if not needed
3. Use shorter samples
4. Close other applications

## Next Steps

- [Installation Guide](INSTALL.md) - Setup and installation details
- [Development Guide](DEVELOPMENT.md) - Architecture and internals
