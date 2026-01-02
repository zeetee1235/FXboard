#pragma once

#include "../audio/AudioEngine.h"
#include "../input/KeyHook.h"
#include "ConfigManager.h"
#include <memory>
#include <atomic>

namespace FXBoard {

/**
 * Core application class for CLI/daemon mode
 * Manages initialization, configuration, and lifecycle of FXBoard
 */
class Application {
public:
    Application();
    ~Application();

    /**
     * Initialize the application
     * @param configPath Path to configuration file
     * @return true if initialization succeeded
     */
    bool initialize(const std::string& configPath = "");

    /**
     * Run the application (blocking until shutdown)
     */
    void run();

    /**
     * Shutdown the application
     */
    void shutdown();

    /**
     * Check if application is running
     */
    bool isRunning() const { return running.load(); }

    /**
     * Request shutdown (can be called from signal handler)
     */
    void requestShutdown() { running.store(false); }

private:
    void loadConfiguration(const std::string& configPath);
    void loadSamples();
    void setupKeyMappings();
    void printStatus();

    std::unique_ptr<AudioEngine> audioEngine;
    std::unique_ptr<KeyHook> keyHook;
    ConfigManager configManager;

    std::atomic<bool> running;
};

} // namespace FXBoard
