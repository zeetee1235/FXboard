#include "Application.h"
#include <juce_core/juce_core.h>
#include <iostream>
#include <thread>
#include <chrono>

namespace FXBoard {

Application::Application() : running(false) {
}

Application::~Application() {
    shutdown();
}

bool Application::initialize(const std::string& configPath) {
    std::cout << "=== FXBoard Initializing ===" << std::endl;
    
    // Load configuration
    loadConfiguration(configPath);
    
    // Initialize audio engine
    audioEngine = std::make_unique<AudioEngine>();
    
    int bufferSize = 128;  // Default low-latency buffer
    if (!audioEngine->initialize(bufferSize)) {
        std::cerr << "Error: Failed to initialize audio device" << std::endl;
        return false;
    }
    std::cout << "✓ Audio engine initialized" << std::endl;
    
    // Initialize keyboard hook
    keyHook = std::make_unique<KeyHook>();
    
    // Load samples
    loadSamples();
    
    // Setup key mappings
    setupKeyMappings();
    
    // Start audio engine
    audioEngine->start();
    std::cout << "✓ Audio engine started" << std::endl;
    
    // Start keyboard hook
    if (!keyHook->start()) {
        std::cerr << "Warning: Keyboard hook failed to start" << std::endl;
        std::cerr << "  Make sure you have permission to access /dev/input/event*" << std::endl;
        std::cerr << "  Run: sudo ./scripts/setup_permissions.sh" << std::endl;
        return false;
    }
    std::cout << "✓ Keyboard hook started" << std::endl;
    
    running.store(true);
    
    printStatus();
    
    return true;
}

void Application::run() {
    std::cout << "\n=== FXBoard Running ===" << std::endl;
    std::cout << "Press Ctrl+C to quit" << std::endl;
    std::cout << "\nListening for keyboard input..." << std::endl;
    
    // Main loop - just wait until shutdown is requested
    while (running.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Could add periodic status updates here if needed
    }
    
    std::cout << "\nShutting down..." << std::endl;
}

void Application::shutdown() {
    if (!running.load()) {
        return;
    }
    
    running.store(false);
    
    if (keyHook) {
        keyHook->stop();
    }
    
    if (audioEngine) {
        audioEngine->stop();
    }
    
    std::cout << "✓ FXBoard shut down cleanly" << std::endl;
}

void Application::loadConfiguration(const std::string& configPath) {
    juce::File configFile;
    
    if (!configPath.empty()) {
        configFile = juce::File(configPath);
    } else {
        // Try multiple locations
        std::vector<std::string> locations = {
            "./config.json",
            "./config/fxboard.json.example",
            "/etc/fxboard/config.json"
        };
        
        for (const auto& loc : locations) {
            juce::File testFile(loc);
            if (testFile.existsAsFile()) {
                configFile = testFile;
                break;
            }
        }
    }
    
    if (configFile.existsAsFile()) {
        configManager.loadConfig(configFile);
        std::cout << "✓ Configuration loaded from: " << configFile.getFullPathName() << std::endl;
    } else {
        std::cout << "⚠ No configuration file found, using defaults" << std::endl;
    }
}

void Application::loadSamples() {
    // Try multiple sample directories
    std::vector<std::string> sampleDirs = {
        "./samples",
        "/usr/local/share/fxboard/samples",
        "./build/samples"
    };
    
    juce::File samplesDir;
    for (const auto& dir : sampleDirs) {
        juce::File testDir(dir);
        if (testDir.exists() && testDir.isDirectory()) {
            samplesDir = testDir;
            break;
        }
    }
    
    if (!samplesDir.exists()) {
        std::cout << "⚠ No samples directory found" << std::endl;
        std::cout << "  Create samples directory and add .wav files" << std::endl;
        return;
    }
    
    std::cout << "Loading samples from: " << samplesDir.getFullPathName() << std::endl;
    
    int loadedCount = 0;
    for (auto& file : samplesDir.findChildFiles(juce::File::findFiles, false, "*.wav")) {
        juce::String sampleId = file.getFileNameWithoutExtension();
        if (audioEngine->getSampleManager().loadSample(sampleId, file)) {
            std::cout << "  ✓ Loaded: " << sampleId << std::endl;
            loadedCount++;
        } else {
            std::cout << "  ✗ Failed: " << sampleId << std::endl;
        }
    }
    
    std::cout << "✓ Loaded " << loadedCount << " samples" << std::endl;
}

void Application::setupKeyMappings() {
    // Map common rhythm game keys to kick sample (as default)
    // These are Linux evdev scancodes
    const std::vector<std::pair<uint32_t, const char*>> defaultMappings = {
        {30, "kick"},   // A
        {31, "snare"},  // S
        {32, "hihat"},  // D
        {33, "clap"},   // F
        {36, "kick"},   // J
        {37, "snare"},  // K
        {38, "hihat"},  // L
        {39, "clap"},   // ; (semicolon)
        {56, "kick"},   // Left Alt
        {57, "snare"},  // Space
    };
    
    for (const auto& [scancode, sampleId] : defaultMappings) {
        audioEngine->mapKeyToSample(scancode, sampleId);
    }
    
    std::cout << "✓ Key mappings configured" << std::endl;
}

void Application::printStatus() {
    std::cout << "\n=== Status ===" << std::endl;
    
    if (audioEngine) {
        std::cout << "Audio:" << std::endl;
        std::cout << "  Samples loaded: " << audioEngine->getSampleManager().getNumSamples() << std::endl;
        std::cout << "  Key mappings: " << audioEngine->getKeyMappings().size() << std::endl;
    }
    
    std::cout << std::endl;
}

} // namespace FXBoard
