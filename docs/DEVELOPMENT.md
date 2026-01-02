# Development Guide

This guide explains FXBoard's architecture and how to contribute to the project.

## Architecture Overview

FXBoard is designed as a real-time audio application with ultra-low latency requirements. The architecture follows these principles:

- **Lock-free design**: Audio thread never blocks
- **Minimal allocations**: No dynamic memory allocation in audio callback
- **Clear separation**: Input, audio, and configuration are decoupled
- **Real-time safety**: Audio thread has highest priority

### System Architecture

```
┌─────────────┐         ┌──────────────┐         ┌─────────────┐
│   Keyboard  │────────▶│  Input Hook  │────────▶│ Lock-free   │
│  (evdev)    │         │   Thread     │         │ Event Queue │
└─────────────┘         └──────────────┘         └──────┬──────┘
                                                          │
                                                          ▼
┌─────────────┐         ┌──────────────┐         ┌──────────────┐
│   Audio     │◀────────│    Audio     │◀────────│  Event       │
│  Hardware   │         │   Callback   │         │  Processing  │
└─────────────┘         │  (RT Thread) │         └──────────────┘
                        └──────────────┘
                               │
                               ▼
                        ┌──────────────┐
                        │   Sample     │
                        │   Playback   │
                        └──────────────┘
                               │
                               ▼
                        ┌──────────────┐
                        │   Effects    │
                        │   Chain      │
                        └──────────────┘
```

## Core Components

### 1. Application (`src/core/Application.cpp`)

Main application lifecycle manager:
- Initializes all subsystems
- Loads configuration and samples
- Handles graceful shutdown
- Manages signal handlers

### 2. Input System (`src/input/KeyHook.cpp`)

Captures keyboard input via Linux evdev:
- Runs in separate thread
- Opens `/dev/input/event*` devices
- Filters for keyboard events
- Pushes events to lock-free queue

**Key Features:**
- Device auto-detection with scoring
- Permission error handling
- Non-blocking reads

### 3. Event Queue (`src/core/EventQueue.h`)

Lock-free MPSC (Multi-Producer Single-Consumer) queue:
- Producers: Input thread
- Consumer: Audio thread
- Fixed-size ring buffer
- No dynamic allocation
- Atomic operations only

### 4. Audio Engine (`src/audio/AudioEngine.cpp`)

Real-time audio processing:
- Receives audio callback from JUCE
- Processes events from queue
- Triggers sample playback
- Applies effects chain
- Handles mixing and limiting

**Critical Path:**
```cpp
audioCallback() {
    1. Process events (key down/up)
    2. Render samples
    3. Apply effects
    4. Mix and limit
}
```

### 5. Sample Manager (`src/audio/SampleManager.cpp`)

Manages audio samples:
- Loads WAV files
- Stores in memory
- Provides polyphonic playback
- Voice management

## Data Flow

### Key Press Event Flow

```
1. User presses key
   └─▶ evdev generates event
       └─▶ Input thread reads event
           └─▶ Push to EventQueue (lock-free)
               └─▶ Audio callback pops event
                   └─▶ Trigger sample playback
                       └─▶ Mix to output
                           └─▶ Audio hardware
```

**Total Latency Budget:**
- Input polling: < 1ms
- Queue transfer: < 0.1ms
- Audio buffer: 1.3-5.3ms (64-256 samples @ 48kHz)
- Hardware: 1-3ms
- **Total: 3-10ms** ✓

## Key Design Patterns

### Lock-Free Event Queue

```cpp
class EventQueue {
    std::array<KeyEvent, 1024> buffer;  // Fixed size
    std::atomic<size_t> tail;           // Consumer position
    size_t head;                         // Producer position
    
    bool push(const KeyEvent& e) {
        // No locks, atomic operations only
        auto next = (head + 1) & mask;
        if (next == tail.load(std::memory_order_acquire))
            return false;  // Queue full
        buffer[head] = e;
        head = next;
        return true;
    }
};
```

### Real-Time Audio Callback

```cpp
void audioCallback(float** output, int numSamples) {
    // NO dynamic allocation
    // NO locks/mutexes
    // NO blocking operations
    // NO system calls
    
    // Process events
    KeyEvent event;
    while (eventQueue.pop(event)) {
        handleEvent(event);
    }
    
    // Render audio
    renderSamples(output, numSamples);
}
```

### Parameter Smoothing

Prevents audio clicks from sudden parameter changes:

```cpp
class Smoother {
    float value, target, coeff;
    
    void setTimeMs(float ms, float sr) {
        coeff = 1.0f / (ms * sr / 1000.0f);
    }
    
    float step() {
        value += coeff * (target - value);
        return value;
    }
};
```

## Building and Testing

### Build Process

```bash
# Debug build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .

# Release build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

### Running Tests

Currently, FXBoard uses manual testing:

1. **Latency Test**: Use external tools to measure input→output delay
2. **Stability Test**: Run for extended periods
3. **CPU Test**: Monitor CPU usage with `top` or `htop`
4. **Memory Test**: Check with `valgrind` or similar

### Debugging

```bash
# Run with GDB
gdb ./build/FXBoard_artefacts/Debug/FXBoard

# Set breakpoints (avoid in audio callback!)
(gdb) break main
(gdb) run

# Check for memory leaks
valgrind --leak-check=full ./FXBoard
```

## Code Style

### General Guidelines

- **C++17** standard
- **RAII** for resource management
- **Smart pointers** for ownership
- **const correctness**
- **Clear naming** (no abbreviations except common ones)

### Example

```cpp
// Good
class AudioEngine {
public:
    bool initialize(int bufferSize);
    void start();
    void stop();
    
private:
    std::unique_ptr<SampleManager> sampleManager;
    std::atomic<bool> running{false};
};

// Avoid
class AudEng {
    void init(int bs);  // Unclear abbreviations
    void go();          // Non-descriptive name
    SampleManager* sm;  // Raw pointer
};
```

### Audio Thread Rules

Code in audio callback MUST follow these rules:

```cpp
void audioCallback() {
    // ✓ OK
    float* buffer = stackBuffer;
    memset(buffer, 0, size);
    atomic.load();
    
    // ✗ FORBIDDEN
    new int[100];           // Dynamic allocation
    mutex.lock();           // Blocking
    malloc(100);            // C allocation
    printf("...");          // System call
    std::vector<int> v;     // May allocate
}
```

## Performance Optimization

### Latency Optimization

1. **Minimize buffer size**: Use 64-128 samples
2. **Real-time thread priority**: Set high priority for audio thread
3. **Lock-free design**: No blocking in audio path
4. **Pre-load samples**: Load all samples at startup

### CPU Optimization

1. **SIMD**: Use vectorized operations where applicable
2. **Branch prediction**: Minimize branches in hot paths
3. **Cache efficiency**: Keep hot data together
4. **Avoid allocations**: Pre-allocate everything

### Memory Optimization

1. **Fixed buffers**: No dynamic sizing
2. **Object pooling**: Reuse voice objects
3. **Sample compression**: Use shorter samples

## Contributing

### Getting Started

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/my-feature`
3. Make your changes
4. Test thoroughly
5. Submit a pull request

### Pull Request Guidelines

- **Clear description**: Explain what and why
- **Small commits**: Logical, focused changes
- **Test changes**: Verify nothing breaks
- **Update docs**: If behavior changes
- **Follow style**: Match existing code style

### Areas for Contribution

**High Priority:**
- Windows input handling (Raw Input API)
- macOS input handling (IOKit)
- Hot-reload configuration
- Real-time thread priority
- Performance profiling tools

**Medium Priority:**
- GUI control panel (optional)
- More effect types
- Profile management
- Latency measurement
- systemd service file

**Low Priority:**
- Plugin system
- MIDI support
- Network control
- Visual feedback

## Architecture Decisions

### Why Lock-Free Queue?

- **No priority inversion**: Audio thread never waits
- **Predictable latency**: No lock contention
- **Real-time safe**: No blocking syscalls

### Why Single Consumer?

- Only audio thread reads events
- Simplifies synchronization
- Better cache performance

### Why JUCE?

- **Cross-platform**: Works on Linux, Windows, macOS
- **Mature**: Well-tested audio code
- **Flexible**: Many audio backends (ALSA, JACK, etc.)

However, JUCE adds complexity. Future versions may use lighter alternatives.

### Why evdev on Linux?

- **Low-level**: Direct kernel access
- **Fast**: No X11 overhead
- **Universal**: Works in any environment
- **Precise**: Raw scancode events

## Future Roadmap

### Phase 2 (Current)
- ✓ Core functionality complete
- ✓ Linux evdev implementation
- ✓ Documentation
- ⧗ Performance optimization
- ⧗ Latency measurement

### Phase 3 (Next)
- Windows support (Raw Input)
- macOS support (IOKit)
- Real-time thread priority
- Hot-reload configuration
- systemd service

### Phase 4 (Future)
- Optional GUI (system tray)
- Profile system
- Advanced effects
- Plugin architecture

## Performance Goals

Current targets:
- **Latency**: < 10ms (key to audio)
- **CPU**: < 5% idle, < 20% under load
- **Memory**: < 50MB (with samples)
- **Stability**: 24/7 operation

## Testing Checklist

Before submitting PR:

- [ ] Builds without warnings
- [ ] No memory leaks (valgrind)
- [ ] Tested with actual keyboard
- [ ] Tested with rhythm game
- [ ] Documentation updated
- [ ] No crashes after 10+ minutes
- [ ] CPU usage acceptable

## Getting Help

- Check existing issues on GitHub
- Read through docs/
- Review code comments
- Ask in discussions

## License

[To be determined]

## Credits

- JUCE framework
- Linux kernel input subsystem
- Contributors (see GitHub)
