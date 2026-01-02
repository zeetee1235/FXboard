#include "MainApp.h"
#include "ModernUI.h"
#include <chrono>

namespace FXBoard {

MainComponent::MainComponent(AudioEngine& ae, KeyHook& kh)
    : audioEngine(ae), keyHook(kh) {
    setSize(1000, 700);
    startTimer(100); // UI update interval

    // 모던 UI 추가
    modernUI = std::make_unique<ModernMainComponent>(&audioEngine);
    addAndMakeVisible(modernUI.get());
    
    // 키보드 포커스 설정
    setWantsKeyboardFocus(true);
    addKeyListener(modernUI.get());
}

MainComponent::~MainComponent() {
    stopTimer();
    removeKeyListener(modernUI.get());
}

void MainComponent::paint(juce::Graphics& g) {
    g.fillAll(UITheme::background);
}

void MainComponent::resized() {
    if (modernUI) {
        modernUI->setBounds(getLocalBounds());
    }
}

void MainComponent::timerCallback() {
    repaint();
}


// ==============================================================================
// MainApp
// ==============================================================================

void MainApp::initialise(const juce::String& commandLine) {
    juce::ignoreUnused(commandLine);
    
    // 오디오 엔진 초기화
    audioEngine = std::make_unique<AudioEngine>();
    if (!audioEngine->initialize(128)) {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "Audio Error",
            "Failed to initialize audio device",
            "OK");
        quit();
        return;
    }
    
    // 키 훅 초기화
    keyHook = std::make_unique<KeyHook>();
    
    // 설정 로드
    juce::File configFile = juce::File::getCurrentWorkingDirectory().getChildFile("config.json");
    
    if (configFile.existsAsFile()) {
        configManager.loadConfig(configFile);
    }
    
    // 샘플 로드
    juce::File samplesDir = juce::File::getCurrentWorkingDirectory().getChildFile("samples");
    
    if (samplesDir.exists()) {
        for (auto& file : samplesDir.findChildFiles(juce::File::findFiles, false, "*.wav")) {
            juce::String sampleId = file.getFileNameWithoutExtension();
            audioEngine->getSampleManager().loadSample(sampleId, file);
            juce::Logger::writeToLog("Loaded sample: " + sampleId);
        }
    }
    
    // 키 매핑 설정 (10개 키에 모두 kick 샘플 할당)
    // A, S, D, F (왼손)
    audioEngine->mapKeyToSample(30, "kick");  // A
    audioEngine->mapKeyToSample(31, "kick");  // S
    audioEngine->mapKeyToSample(32, "kick");  // D
    audioEngine->mapKeyToSample(33, "kick");  // F
    // J, K, L, ; (오른손)
    audioEngine->mapKeyToSample(36, "kick");  // J
    audioEngine->mapKeyToSample(37, "kick");  // K
    audioEngine->mapKeyToSample(38, "kick");  // L
    audioEngine->mapKeyToSample(39, "kick");  // ; (semicolon)
    // Alt, Space (엄지)
    audioEngine->mapKeyToSample(56, "kick");  // Left Alt
    audioEngine->mapKeyToSample(57, "kick");  // Space
    
    juce::Logger::writeToLog("Key mappings configured: A,S,D,F,J,K,L,;,Alt,Space -> kick");
    
    // 오디오 시작
    audioEngine->start();
    
    // 키 훅 시작 (백그라운드에서 동작)
    if (!keyHook->start()) {
        juce::Logger::writeToLog("Warning: Key hook failed to start");
    }
    
    // 메뉴 모델 생성 (백그라운드 제어용)
    menuModel = std::make_unique<AppMenuModel>(this);
    
    // 명령줄 인자에 따라 시작 방식 결정
    if (commandLine.contains("--hidden") || commandLine.contains("-h")) {
        juce::Logger::writeToLog("Starting in background mode");
        // 백그라운드 모드로 시작 (창 표시 안 함)
        // 사용자는 시스템 메뉴를 통해 창을 열 수 있음
    } else {
        // GUI 창 생성
        showMainWindow();
    }
}

void MainApp::shutdown() {
    // 창들 정리
    menuModel = nullptr;
    settingsWindow = nullptr;
    mainWindow = nullptr;
    
    if (keyHook) {
        keyHook->stop();
    }
    
    if (audioEngine) {
        audioEngine->stop();
    }
}

void MainApp::systemRequestedQuit() {
    quit();
}

void MainApp::anotherInstanceStarted(const juce::String& commandLine) {
    juce::ignoreUnused(commandLine);
    // 이미 실행 중이면 메인 창 표시
    showMainWindow();
}

void MainApp::showMainWindow() {
    juce::Logger::writeToLog("showMainWindow called");
    if (!mainWindow) {
        juce::Logger::writeToLog("Creating new main window");
        auto* mainComp = new MainComponent(*audioEngine, *keyHook);
        mainWindow.reset(new MainWindow(getApplicationName(), mainComp));
        juce::Logger::writeToLog("Main window created and should be visible");
    } else {
        juce::Logger::writeToLog("Main window already exists, showing it");
        mainWindow->setVisible(true);
        mainWindow->toFront(true);
    }
}

void MainApp::hideMainWindow() {
    if (mainWindow) {
        mainWindow->setVisible(false);
    }
}

void MainApp::showSettingsWindow() {
    if (!settingsWindow) {
        settingsWindow.reset(new SettingsWindow(audioEngine.get(), &configManager));
    } else {
        settingsWindow->setVisible(true);
        settingsWindow->toFront(true);
    }
}

void MainApp::quitApplication() {
    systemRequestedQuit();
}

} // namespace FXBoard
