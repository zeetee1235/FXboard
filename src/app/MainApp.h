#pragma once
#include "../audio/AudioEngine.h"
#include "../input/KeyHook.h"
#include "ConfigManager.h"
#include "ModernUI.h"
#include "SettingsWindow.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace FXBoard {

/**
 * 메인 GUI 컴포넌트
 */
class MainComponent : public juce::Component, 
                      public juce::Timer {
public:
    MainComponent(AudioEngine& ae, KeyHook& kh);
    ~MainComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    
private:
    AudioEngine& audioEngine;
    KeyHook& keyHook;
    
    std::unique_ptr<ModernMainComponent> modernUI;
    
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
    
    // 시스템 트레이 메뉴
    void showMainWindow();
    void hideMainWindow();
    void showSettingsWindow();
    void quitApplication();
    
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
            
            // 컴포넌트 크기에 맞춰 창 크기 설정
            centreWithSize(1000, 700);
            setVisible(true);
        }
        
        void closeButtonPressed() override {
            // 창을 닫으면 백그라운드로 전환
            setVisible(false);
        }
        
    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };
    
    /**
     * 메뉴 바 모델 (시스템 트레이 대신)
     */
    class AppMenuModel : public juce::MenuBarModel {
    public:
        AppMenuModel(MainApp* app) : mainApp(app) {}
        
        juce::StringArray getMenuBarNames() override {
            return {"FXBoard"};
        }
        
        juce::PopupMenu getMenuForIndex(int menuIndex, const juce::String&) override {
            juce::PopupMenu menu;
            
            if (menuIndex == 0) {
                menu.addItem(1, "Show Main Window");
                menu.addItem(2, "Settings");
                menu.addSeparator();
                menu.addItem(3, "Quit");
            }
            
            return menu;
        }
        
        void menuItemSelected(int menuItemID, int) override {
            if (menuItemID == 1) {
                mainApp->showMainWindow();
            } else if (menuItemID == 2) {
                mainApp->showSettingsWindow();
            } else if (menuItemID == 3) {
                mainApp->quitApplication();
            }
        }
        
    private:
        MainApp* mainApp;
    };
    
private:
    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<SettingsWindow> settingsWindow;
    std::unique_ptr<AppMenuModel> menuModel;
    std::unique_ptr<AudioEngine> audioEngine;
    std::unique_ptr<KeyHook> keyHook;
    ConfigManager configManager;
};

} // namespace FXBoard
