# FXBoard TODO

## Phase 1: Core Functionality (COMPLETED)
- [x] CLI/daemon mode implementation
- [x] Remove excessive UI code
- [x] Improve Linux evdev keyboard input
- [x] Simplify build system
- [x] Consolidate documentation
- [x] Build and test successfully

## Phase 2: Performance & Optimization (IN PROGRESS)
- [ ] Add real-time thread priority for audio
- [ ] Implement latency measurement and reporting
- [ ] Optimize event queue processing
- [ ] Add CPU usage monitoring
- [ ] Memory usage profiling
- [ ] Buffer underrun prevention improvements

## Phase 3: Platform Support
- [ ] Windows input handling (Raw Input API)
- [ ] macOS input handling (IOKit)
- [ ] Cross-platform testing

## Phase 4: Features
- [ ] Hot-reload configuration
- [ ] systemd service file
- [ ] Profile management (multiple configs)
- [ ] Sample volume adjustment at runtime
- [ ] Additional effects (chorus, delay)
- [ ] MIDI input support (optional)

## Phase 5: UI (Optional)
- [ ] Simple system tray GUI
- [ ] Visual key mapping editor
- [ ] Sample management GUI
- [ ] Real-time latency display
- [ ] CPU/memory monitor

## Quality & Testing
- [ ] Extended stability testing (24h run)
- [ ] Latency benchmarking
- [ ] CPU usage benchmarking
- [ ] Memory leak testing
- [ ] Unit tests for core components
- [ ] Integration tests

## Documentation
- [ ] Video tutorial
- [ ] Sample pack recommendations
- [ ] Troubleshooting guide expansion
- [ ] Performance tuning guide
- [ ] Contributing guidelines

## Known Issues
- Audio initialization warnings (ALSA seq) - harmless, can be suppressed
- Some compiler warnings about sign conversion - non-critical

## Future Ideas
- Plugin system for custom effects
- Network control (OSC/MIDI over network)
- Multiple audio output routing
- Sample recording/editing
- Integration with DAWs