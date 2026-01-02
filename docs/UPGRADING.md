# Upgrading to FXBoard 0.2.0

This guide helps users migrate from FXBoard 0.1.x to the refactored 0.2.0 release.

## What's Changed

Version 0.2.0 is a **major refactoring** that removes the GUI and focuses on core functionality. The application now runs as a command-line/daemon process instead of a GUI application.

## Key Differences

| Feature | 0.1.x (Old) | 0.2.0 (New) |
|---------|-------------|-------------|
| Interface | GUI Window | Command-line |
| Execution | Opens window | Runs in terminal |
| Configuration | GUI + JSON | JSON only |
| Dependencies | X11, GUI libs | Audio libs only |
| Binary Size | ~10MB+ | ~2MB |
| Performance | Higher overhead | Optimized |

## Migration Steps

### 1. Backup Your Configuration

```bash
# Save your old config
cp config.json config.json.backup
```

### 2. Rebuild

```bash
# Clean old build
rm -rf build/

# Build new version
./scripts/build.sh
```

### 3. Setup Permissions (Required)

This is a **new requirement** for 0.2.0:

```bash
sudo ./scripts/setup_permissions.sh
# Then log out and log back in!
```

### 4. Test the New Version

```bash
# Run the test suite
./scripts/test.sh

# Try running (will show initialization)
./build/FXBoard_artefacts/Release/FXBoard
```

### 5. Install System-Wide (Optional)

```bash
sudo ./scripts/install.sh
# Then run with: fxboard
```

## Configuration Changes

### What Stays the Same

The configuration file format is **unchanged**. Your existing `config.json` will work with 0.2.0:

```json
{
  "audio": { ... },
  "keymapping": { ... },
  "samples": { ... },
  "fx": { ... }
}
```

### What's Different

- **No GUI settings**: All GUI-related config is ignored
- **Config locations**: Now checks multiple paths (see below)
- **No hot-reload**: Must restart to apply config changes

### Config Search Order

FXBoard now looks for configuration in this order:

1. Path specified with `-c` flag
2. `./config.json` (current directory)
3. `./config/fxboard.json.example`
4. `/etc/fxboard/config.json` (after system install)

## Usage Changes

### Old Way (0.1.x)

```bash
# GUI window opens automatically
./FXBoard
```

### New Way (0.2.0)

```bash
# Runs in terminal with output
./build/FXBoard_artefacts/Release/FXBoard

# Or after system install
fxboard

# Custom config
fxboard -c /path/to/config.json

# Show help
fxboard --help

# Stop with Ctrl+C
```

## Feature Comparison

### Removed Features

- ❌ GUI window
- ❌ Settings window
- ❌ Visual key mapping editor
- ❌ GUI-based sample management
- ❌ System tray icon

### Retained Features

- ✅ Low-latency audio playback
- ✅ Keyboard input capture (evdev)
- ✅ Sample loading and management
- ✅ Key mapping configuration
- ✅ Audio effects (filter, bitcrusher, reverb)
- ✅ JSON configuration

### New Features

- ✅ Command-line interface
- ✅ Signal handling (Ctrl+C)
- ✅ Better permission management
- ✅ Automated installation scripts
- ✅ Comprehensive documentation
- ✅ Test suite

## Troubleshooting

### "GUI doesn't open"

**Expected behavior** - there is no GUI in 0.2.0. The application runs in the terminal.

### "Permission denied" errors

Run the permission setup script:
```bash
sudo ./scripts/setup_permissions.sh
# Log out and back in!
```

### "Can't find configuration"

0.2.0 searches multiple locations. Use `-c` to specify:
```bash
fxboard -c /path/to/your/config.json
```

### "Audio initialization failed"

Check that audio system is running:
```bash
# For PulseAudio
pactl info

# For ALSA
aplay -l
```

### "Samples not loading"

Check samples directory:
```bash
ls -la samples/*.wav
```

Generate test samples if needed:
```bash
python3 generate_samples_simple.py
```

## Reverting to 0.1.x

If you need to go back to the GUI version:

```bash
# Checkout the old version
git checkout <previous-version-tag>

# Rebuild
rm -rf build
./build.sh
```

## Getting Help

- **Documentation**: See `docs/` directory
  - Installation: `docs/INSTALL.md`
  - Configuration: `docs/CONFIG.md`
  - Development: `docs/DEVELOPMENT.md`
- **Issues**: Open an issue on GitHub
- **Changelog**: See `CHANGELOG.md` for all changes

## FAQ

**Q: Why remove the GUI?**
A: To focus on core functionality, reduce complexity, and optimize performance. A simple GUI may be added back in Phase 5 as an optional component.

**Q: Will the GUI come back?**
A: A simple system tray GUI may be added in the future as an optional feature. See `TODO.MD` for roadmap.

**Q: Is 0.2.0 faster?**
A: Yes, removing GUI overhead improves performance and reduces latency.

**Q: Can I use my old config file?**
A: Yes! The JSON format is unchanged. Just copy your config.json.

**Q: Do I need to reinstall dependencies?**
A: No, but you may be able to remove GUI-related dependencies if not used by other programs.

**Q: Can I run multiple instances?**
A: Yes, but they will compete for keyboard input. Use different configs if needed.

## Recommendations

For best experience with 0.2.0:

1. ✅ Run the test script: `./scripts/test.sh`
2. ✅ Set up permissions properly
3. ✅ Read the new documentation
4. ✅ Test thoroughly before using in actual gameplay
5. ✅ Report any issues on GitHub

## Next Steps

After migrating:

1. Read the new documentation in `docs/`
2. Customize your configuration (see `docs/CONFIG.md`)
3. Test latency with actual gameplay
4. Consider system-wide installation
5. Provide feedback on GitHub

---

**Note**: If you encounter any issues during migration, please open an issue on GitHub with:
- Version you're upgrading from
- Error messages
- Output of `./scripts/test.sh`
