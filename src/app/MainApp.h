#pragma once
#include "../audio/AudioEngine.h"
#include "../input/KeyHook.h"
#include "ConfigManager.h"
#include "RhythmGameUI.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace FXBoard {

/**
 * 메인 GUI 컴포넌트
 */
class MainComponent : public juce::Component, 
                      public juce::Timer,
                      public juce::KeyListener {
public:
    MainComponent(AudioEngine& ae, KeyHook& kh);
    ~MainComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    
    // KeyListener 오버라이드
    bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override;
    bool keyStateChanged(bool isKeyDown, juce::Component* originatingComponent) override;
    
private:
    AudioEngine& audioEngine;
    KeyHook& keyHook;
    
    // UI 상태
    double cpuLoad = 0.0;
    int xrunCount = 0;
    bool isRunning = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

/**
 * 메인 애플리케이션
 */
class MainApp : public juce::JUCEApplication {
public:
    MainApp() {}
    
    const juce::String getApplicationName() override { return "FXBoard"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }
    bool moreThanOneInstanceAllowed() override { return false; }
    
    void initialise(const juce::String& commandLine) override;
    void shutdown() override;
    
    void systemRequestedQuit() override;
    void anotherInstanceStarted(const juce::String& commandLine) override;
    
    class MainWindow : public juce::DocumentWindow {
    public:
        MainWindow(juce::String name, MainComponent* content)
            : DocumentWindow(name,
                           juce::Desktop::getInstance().getDefaultLookAndFeel()
                               .findColour(juce::ResizableWindow::backgroundColourId),
                           DocumentWindow::allButtons) {
            setUsingNativeTitleBar(true);
            setContentOwned(content, true);
            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
            setVisible(true);
        }
        
        void closeButtonPressed() override {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
        
    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };
    
private:
    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<AudioEngine> audioEngine;
    std::unique_ptr<KeyHook> keyHook;
    ConfigManager configManager;
};

} // namespace FXBoard
