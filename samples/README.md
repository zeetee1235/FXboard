# FXBoard Samples Directory

Place your WAV audio samples in this directory.

## Quick Start

### Generate Test Samples

Use the included Python script to generate basic test samples:

```bash
cd ..
python3 generate_samples_simple.py
```

This will create:
- `kick.wav` - Kick drum
- `snare.wav` - Snare drum
- `hihat.wav` - Hi-hat
- `clap.wav` - Hand clap

### Use Your Own Samples

Copy your WAV files here:

```bash
cp /path/to/your/samples/*.wav .
```

## Sample Requirements

- **Format**: WAV
- **Sample Rate**: Any (automatically resampled to match audio output)
- **Bit Depth**: 16-bit or 24-bit recommended
- **Channels**: Mono or stereo
- **Duration**: Short samples (< 2 seconds) recommended for best latency

## Sample Naming

Sample filenames (without .wav extension) are used as sample IDs in the configuration file.

Example:
- File: `kick.wav`
- Sample ID: `kick`
- Config: `"30": "kick"` (maps key A to kick sample)

## Recommended Sample Properties

- **Sample Rate**: 48kHz (or 44.1kHz)
- **Bit Depth**: 16-bit or 24-bit
- **Channels**: Mono or stereo
- **Duration**: 0.5 - 5 seconds (shorter = more memory efficient)

## Example Sample Sets

For rhythm games:

**Sound Voltex style:**
- BT button sounds (4 different tones)
- FX laser sounds (2 types)

**Beatmania style:**
- Scratch sound
- Key sounds (7 keys)

**Generic percussion:**
- `kick.wav` - Kick drum
- `snare.wav` - Snare drum
- `hihat.wav` - Hi-hat
- `clap.wav` - Hand clap
- `crash.wav` - Crash cymbal
- `tom.wav` - Tom

## Tips

- **Keep it short**: Shorter samples = lower memory usage
- **Normalize volume**: Prevent clipping by keeping peaks below 0dB
- **Use quality samples**: Good source material = good results
- **Test volume balance**: Ensure all samples are at similar volumes

## Free Sample Resources

- Freesound.org
- 99Sounds.org
- Bedroom Producers Blog (Free Sample Packs)
- LMMS sample library
- Hydrogen drum kit samples
