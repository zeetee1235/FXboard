#include "MainApp.h"
#include "RhythmGameUI.h"
#include <chrono>

namespace FXBoard {

MainComponent::MainComponent(AudioEngine& ae, KeyHook& kh)
    : audioEngine(ae), keyHook(kh) {
    setSize(800, 600);
    startTimer(100); // 100ms마다 UI 업데이트
    
    // 키 리스너 등록
    setWantsKeyboardFocus(true);
    addKeyListener(this);
}

MainComponent::~MainComponent() {
    stopTimer();
    removeKeyListener(this);
}

void MainComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black);
    
    g.setColour(juce::Colours::white);
    g.setFont(28.0f);
    g.drawText("FXBoard - Rhythm Game Setup", 
               getLocalBounds().reduced(20).removeFromTop(50),
               juce::Justification::centred);
    
    // 상태 표시
    int y = 80;
    g.setFont(16.0f);
    
    g.drawText("Status: " + juce::String(isRunning ? "🟢 Running" : "🔴 Stopped"),
               20, y, getWidth() - 40, 30, juce::Justification::left);
    y += 35;
    
    g.drawText("CPU Load: " + juce::String(cpuLoad, 1) + "%",
               20, y, getWidth() - 40, 30, juce::Justification::left);
    y += 35;
    
    double latency = audioEngine.getLatencyMs();
    juce::Colour latencyColor = latency < 10.0 ? juce::Colours::green : 
                                 latency < 20.0 ? juce::Colours::yellow : juce::Colours::red;
    g.setColour(latencyColor);
    g.drawText("Latency: " + juce::String(latency, 2) + " ms",
               20, y, getWidth() - 40, 30, juce::Justification::left);
    y += 35;
    
    g.setColour(juce::Colours::white);
    g.drawText("XRuns: " + juce::String(xrunCount),
               20, y, getWidth() - 40, 30, juce::Justification::left);
    y += 35;
    
    g.drawText("Key Hook: " + juce::String(keyHook.isActive() ? "✓ Active" : "✗ Inactive (using GUI keys)"),
               20, y, getWidth() - 40, 30, juce::Justification::left);
    
    // 간단한 사용법
    y += 60;
    g.setFont(14.0f);
    g.setColour(juce::Colours::lightgreen);
    g.drawText("🎮 Rhythm Game Controls:", 20, y, getWidth() - 40, 30, juce::Justification::left);
    y += 30;
    g.setColour(juce::Colours::lightgrey);
    g.drawText("• Press A, S, D, F to trigger drum sounds", 40, y, getWidth() - 60, 25, juce::Justification::left);
    y += 25;
    g.drawText("• Ultra-low latency for perfect timing", 40, y, getWidth() - 60, 25, juce::Justification::left);
    y += 25;
    g.drawText("• Configure more keys in config.json", 40, y, getWidth() - 60, 25, juce::Justification::left);
    y += 30;
    
    // 키 매핑 표시
    g.setColour(juce::Colours::cyan);
    g.drawText("Current Key Mappings:", 20, y, getWidth() - 40, 25, juce::Justification::left);
    y += 25;
    g.setFont(12.0f);
    g.setColour(juce::Colours::white);
    g.drawText("A = Kick  |  S = Snare  |  D = Hi-Hat  |  F = Clap", 
               40, y, getWidth() - 60, 20, juce::Justification::left);
}

void MainComponent::resized() {
    // UI 레이아웃
}

void MainComponent::timerCallback() {
    cpuLoad = audioEngine.getCpuLoad();
    xrunCount = audioEngine.getXRunCount();
    isRunning = keyHook.isActive();
    repaint();
}

bool MainComponent::keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) {
    juce::ignoreUnused(originatingComponent);
    
    // JUCE KeyPress를 스캔코드로 변환
    int keyCode = key.getKeyCode();
    
    // A, S, D, F 키 처리
    uint32_t scancode = 0;
    bool matched = false;
    
    if (keyCode == 'A' || keyCode == 'a') { scancode = 30; matched = true; }
    else if (keyCode == 'S' || keyCode == 's') { scancode = 31; matched = true; }
    else if (keyCode == 'D' || keyCode == 'd') { scancode = 32; matched = true; }
    else if (keyCode == 'F' || keyCode == 'f') { scancode = 33; matched = true; }
    
    if (matched) {
        // 이벤트를 AudioEngine에 전달
        auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        KeyEvent event(KeyEvent::Down, scancode, static_cast<uint64_t>(now));
        audioEngine.getEventQueue().push(event);
        
        juce::Logger::writeToLog("Key pressed: " + juce::String((char)keyCode) + 
                                " (scancode " + juce::String(scancode) + ")");
        return true;
    }
    
    return false;
}

bool MainComponent::keyStateChanged(bool isKeyDown, juce::Component* originatingComponent) {
    juce::ignoreUnused(isKeyDown, originatingComponent);
    return false;
}

// MainApp 구현

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
    juce::File configFile = juce::File::getSpecialLocation(
        juce::File::currentApplicationFile).getParentDirectory().getChildFile("config.json");
    
    if (configFile.existsAsFile()) {
        configManager.loadConfig(configFile);
    }
    
    // 샘플 로드 예제 (나중에 설정 파일에서 로드)
    juce::File samplesDir = juce::File::getSpecialLocation(
        juce::File::currentApplicationFile).getParentDirectory().getChildFile("samples");
    
    if (samplesDir.exists()) {
        for (auto& file : samplesDir.findChildFiles(juce::File::findFiles, false, "*.wav")) {
            juce::String sampleId = file.getFileNameWithoutExtension();
            audioEngine->getSampleManager().loadSample(sampleId, file);
            juce::Logger::writeToLog("Loaded sample: " + sampleId);
        }
    }
    
    // 키 매핑 설정 (config.json 기준)
    // A(30)->kick, S(31)->snare, D(32)->hihat, F(33)->clap
    audioEngine->mapKeyToSample(30, "kick");
    audioEngine->mapKeyToSample(31, "snare");
    audioEngine->mapKeyToSample(32, "hihat");
    audioEngine->mapKeyToSample(33, "clap");
    
    juce::Logger::writeToLog("Key mappings configured:");
    juce::Logger::writeToLog("  A(30) -> kick");
    juce::Logger::writeToLog("  S(31) -> snare");
    juce::Logger::writeToLog("  D(32) -> hihat");
    juce::Logger::writeToLog("  F(33) -> clap");
    
    // 오디오 시작
    audioEngine->start();
    
    // 키 훅 시작
    if (!keyHook->start()) {
        juce::Logger::writeToLog("Warning: Key hook failed to start");
    }
    
    // GUI 생성
    auto* mainComp = new MainComponent(*audioEngine, *keyHook);
    mainWindow.reset(new MainWindow(getApplicationName(), mainComp));
}

void MainApp::shutdown() {
    if (keyHook) {
        keyHook->stop();
    }
    
    if (audioEngine) {
        audioEngine->stop();
    }
    
    mainWindow = nullptr;
}

void MainApp::systemRequestedQuit() {
    quit();
}

void MainApp::anotherInstanceStarted(const juce::String& commandLine) {
    juce::ignoreUnused(commandLine);
}

} // namespace FXBoard
