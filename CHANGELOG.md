# Changelog

All notable changes to FXBoard will be documented in this file.

## [0.2.0] - 2026-01-02

### Major Refactoring - Phase 1

This release represents a complete refactoring of FXBoard, focusing on core functionality and removing unnecessary complexity.

### Added
- **CLI/Daemon Mode**: Complete rewrite for command-line operation without GUI
- **New Application Class**: Simplified `Application.cpp/h` replacing complex MainApp
- **Signal Handlers**: Graceful shutdown with SIGINT/SIGTERM
- **Comprehensive Documentation**:
  - docs/INSTALL.md - Detailed installation guide
  - docs/CONFIG.md - Configuration reference
  - docs/DEVELOPMENT.md - Architecture and contribution guide
  - Rewritten README.md with quick start
- **Build Scripts**:
  - scripts/build.sh - Simplified build process
  - scripts/install.sh - System-wide installation
  - scripts/setup_permissions.sh - Automated permission setup
  - scripts/test.sh - Basic test suite
- **Permission Management**:
  - config/99-fxboard.rules - udev rules for input access
  - Automated group management scripts
- **Example Configuration**: config/fxboard.json.example
- **Improved Keyboard Detection**: Device scoring algorithm to find best keyboard

### Changed
- **Build System**: Switched from `juce_add_gui_app` to `juce_add_console_app`
- **Removed GUI Dependencies**: No longer requires X11, graphics, or GUI modules
- **Simplified CMakeLists.txt**: Removed unnecessary complexity
- **Binary Size**: Reduced from GUI version to 2.1MB console app
- **Keyboard Input**: Enhanced evdev device detection with better error messages
- **Configuration Loading**: Multiple search paths for flexibility

### Removed
- All GUI components:
  - src/app/ModernUI.h
  - src/app/RhythmGameUI.h
  - src/app/SettingsWindow.h/cpp
  - src/app/MainApp.h/cpp
- Duplicate documentation files:
  - QUICKSTART.md, QUICK_START.md
  - USAGE.md, RUNNING.md
  - BACKGROUND_MODE.md, BUILD_SUCCESS.md
  - KEYBOARD_GUIDE.md, MODERN_UI_GUIDE.md
  - RHYTHM_GAME_GUIDE.md, SAMPLES_GUIDE.md
- X11 dependency
- GUI JUCE modules (juce_gui_basics, juce_gui_extra, juce_graphics)
- Approximately 3,443 lines of code removed, 1,895 added (net -1,548 lines)

### Fixed
- Include path issues in Application.h
- Bit-shift overflow warning in keyboard device detection
- Build configuration for console-only mode

### Performance
- Audio engine initializes at 48kHz with 128 sample buffer (~2.67ms latency)
- 12 samples load successfully
- Clean startup and shutdown

### Known Issues
- ALSA sequencer warnings (harmless, can be ignored)
- Requires proper permissions for keyboard access (/dev/input/event*)
- Some compiler warnings about sign conversions (non-critical)

### Testing
- ✅ Build: Success (2.1MB binary)
- ✅ Audio initialization: Working
- ✅ Sample loading: Working (12 samples)
- ✅ Configuration loading: Working
- ✅ Keyboard detection: Working (correctly reports permission issues)
- ✅ Help flag: Working
- ⏳ Actual keyboard input: Requires permission setup
- ⏳ Latency measurement: Pending testing
- ⏳ Long-term stability: Needs extended testing

### Migration Guide

For users upgrading from 0.1.x:

1. **No GUI**: All configuration is now via JSON files only
2. **New Execution**:
   - Old: GUI window opens automatically
   - New: `./FXBoard` runs in terminal
3. **Permissions Required**: 
   - Run `sudo ./scripts/setup_permissions.sh`
   - Log out and back in
4. **New Config Locations**: See docs/CONFIG.md for details

### Next Steps (Phase 2)
- Real-time thread priority
- Latency measurement and reporting
- Performance optimization
- systemd service file
- Windows/macOS support

## [0.1.0] - Previous Release

Initial release with GUI, basic functionality.

---

## Version Numbering

FXBoard follows [Semantic Versioning](https://semver.org/):
- MAJOR version for incompatible API changes
- MINOR version for new functionality in a backward compatible manner
- PATCH version for backward compatible bug fixes
