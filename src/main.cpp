#include "core/Application.h"
#include <csignal>
#include <iostream>
#include <cstring>

// Global application instance for signal handler
FXBoard::Application* g_app = nullptr;

// Signal handler for graceful shutdown
void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    if (g_app) {
        g_app->requestShutdown();
    }
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::string configPath;
    bool showHelp = false;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            showHelp = true;
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--config") == 0) {
            if (i + 1 < argc) {
                configPath = argv[++i];
            }
        }
    }
    
    if (showHelp) {
        std::cout << "FXBoard - Low-latency keyboard sound program for rhythm games" << std::endl;
        std::cout << "\nUsage: " << argv[0] << " [options]" << std::endl;
        std::cout << "\nOptions:" << std::endl;
        std::cout << "  -h, --help              Show this help message" << std::endl;
        std::cout << "  -c, --config <path>     Use specified config file" << std::endl;
        std::cout << "\nDefault config locations:" << std::endl;
        std::cout << "  ./config.json" << std::endl;
        std::cout << "  ./config/fxboard.json.example" << std::endl;
        std::cout << "  /etc/fxboard/config.json" << std::endl;
        std::cout << "\nSamples directories:" << std::endl;
        std::cout << "  ./samples" << std::endl;
        std::cout << "  /usr/local/share/fxboard/samples" << std::endl;
        return 0;
    }
    
    // Setup signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Create and initialize application
    FXBoard::Application app;
    g_app = &app;
    
    if (!app.initialize(configPath)) {
        std::cerr << "Failed to initialize FXBoard" << std::endl;
        return 1;
    }
    
    // Run application (blocks until shutdown)
    app.run();
    
    // Cleanup
    app.shutdown();
    g_app = nullptr;
    
    return 0;
}
