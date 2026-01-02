#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include "../audio/AudioEngine.h"

namespace FXBoard {

// A modern, bright color theme
struct Theme {
    static inline const juce::Colour background = juce::Colour(0xfff0f2f5);
    static inline const juce::Colour text = juce::Colour(0xff333333);
    static inline const juce::Colour primary = juce::Colour(0xff007aff);
    static inline const juce::Colour highlight = juce::Colour(0xffffd600);
    static inline const juce::Colour keyButton = juce::Colour(0xffffffff);
    static inline const juce::Colour keyButtonText = juce::Colour(0xff444444);
    static inline const juce::Colour sampleText = juce::Colour(0xff34c759);
};


/**
 * 리듬게임 키 설정 UI (Modern Look)
 */
class RhythmGameUI : public juce::Component,
                     public juce::KeyListener,
                     public juce::Timer {
public:
    RhythmGameUI(AudioEngine* engine) : audioEngine(engine) {
        setSize(900, 650);
        
        setupKeyButtons();
        setupSampleList();
        setupSettingsPanel();
        
        startTimer(16); // ~60fps
    }
    
    ~RhythmGameUI() override {
        stopTimer();
    }
    
    void paint(juce::Graphics& g) override {
        // Background
        g.fillAll(Theme::background);
        
        // Title
        g.setColour(Theme::text);
        g.setFont(juce::Font(28.0f, juce::Font::bold));
        g.drawText("FXBoard", getLocalBounds().removeFromTop(60), juce::Justification::centred);
        
        // Status indicators
        auto statusArea = getLocalBounds().removeFromTop(30);
        g.setFont(14.0f);
        
        juce::String latencyText = "Latency: " + juce::String(audioEngine->getLatencyMs(), 2) + " ms";
        g.setColour(audioEngine->getLatencyMs() < 10.0 ? Theme::sampleText : juce::Colours::orange);
        g.drawText(latencyText, statusArea.removeFromLeft(150).reduced(10, 0), juce::Justification::centredLeft);
        
        juce::String cpuText = "CPU: " + juce::String(audioEngine->getCpuLoad(), 1) + "%";
        g.setColour(Theme::text.withAlpha(0.7f));
        g.drawText(cpuText, statusArea.removeFromLeft(100).reduced(10, 0), juce::Justification::centredLeft);
    }
    
    void resized() override {
        auto bounds = getLocalBounds();
        bounds.removeFromTop(80); // Header area
        
        auto rightPanel = bounds.removeFromRight(300);
        
        // Right Panel: Samples and Settings
        sampleListBox.setBounds(rightPanel.reduced(10).removeFromTop(400));
        settingsArea = rightPanel.reduced(10);
        layoutSettings(settingsArea);

        // Left Panel: Keyboard Layout
        auto keyboardArea = bounds.reduced(20);
        layoutKeyButtons(keyboardArea);
    }
    
    bool keyPressed(const juce::KeyPress& key, juce::Component*) override {
        int keyCode = key.getKeyCode();
        int scancode = getScancodeFromKeyCode(keyCode);
        
        if (scancode >= 0) {
            highlightKey(scancode);
            
            KeyEvent event;
            event.type = KeyEvent::Down;
            event.scancode = scancode;
            event.timestampNs = juce::Time::getHighResolutionTicks();
            audioEngine->pushEvent(event);
            
            return true;
        }
        return false;
    }
    
    bool keyStateChanged(bool isKeyDown, juce::Component*) override {
        return false;
    }
    
    void timerCallback() override {
        for (auto& [scancode, button] : keyButtons) {
            if (keyHighlights[scancode] > 0.0f) {
                keyHighlights[scancode] -= 0.05f;
                if (keyHighlights[scancode] < 0.0f) {
                    keyHighlights[scancode] = 0.0f;
                }
                button->repaint();
            }
        }
    }

private:
    AudioEngine* audioEngine;
    
    struct KeyButton : public juce::TextButton {
        int scancode;
        float& highlight;
        juce::String assignedSample;
        
        KeyButton(int sc, float& hl) : scancode(sc), highlight(hl) {}

        void paint(juce::Graphics& g) override {
            auto bounds = getLocalBounds().toFloat();
            
            // Shadow
            g.setColour(juce::Colours::black.withAlpha(0.1f));
            g.fillRoundedRectangle(bounds.translated(0, 2), 8.0f);

            // Background
            juce::Colour baseColour = Theme::keyButton;
            if (highlight > 0.0f) {
                baseColour = baseColour.interpolatedWith(Theme::highlight, highlight);
            }
            g.setColour(baseColour);
            g.fillRoundedRectangle(bounds, 8.0f);
            
            // Border
            g.setColour(Theme::background.darker(0.1f));
            g.drawRoundedRectangle(bounds.reduced(0.5f), 8.0f, 1.0f);
            
            // Key Name
            g.setColour(Theme::keyButtonText);
            g.setFont(juce::Font(18.0f, juce::Font::bold));
            g.drawText(getButtonText(), bounds.reduced(5.0f).removeFromTop(25.0f), juce::Justification::centred);
            
            // Assigned Sample
            if (assignedSample.isNotEmpty()) {
                g.setFont(12.0f);
                g.setColour(Theme::sampleText);
                g.drawText(assignedSample, bounds.reduced(5.0f), juce::Justification::centredBottom);
            }
        }
    };
    
    std::map<int, std::unique_ptr<KeyButton>> keyButtons;
    std::map<int, float> keyHighlights;
    
    juce::ListBox sampleListBox;
    juce::Rectangle<int> settingsArea;
    
    void setupKeyButtons() {
        createKeyRow({2,3,4,5,6,7,8,9,10,11}, {"1","2","3","4","5","6","7","8","9","0"});
        createKeyRow({16,17,18,19,20,21,22,23,24,25}, {"Q","W","E","R","T","Y","U","I","O","P"});
        createKeyRow({30,31,32,33,34,35,36,37,38}, {"A","S","D","F","G","H","J","K","L"});
        createKeyRow({44,45,46,47,48,49,50}, {"Z","X","C","V","B","N","M"});
    }
    
    void createKeyRow(const std::vector<int>& scancodes, const std::vector<juce::String>& labels) {
        for (size_t i = 0; i < scancodes.size(); ++i) {
            keyHighlights[scancodes[i]] = 0.0f;
            auto button = std::make_unique<KeyButton>(scancodes[i], keyHighlights[scancodes[i]]);
            button->setButtonText(labels[i]);
            addAndMakeVisible(button.get());
            keyButtons[scancodes[i]] = std::move(button);
        }
    }
    
    void layoutKeyButtons(juce::Rectangle<int> area) {
        int keySize = 55;
        int spacing = 8;
        int startY = area.getY();
        
        layoutRow({2,3,4,5,6,7,8,9,10,11}, area, startY, keySize, spacing);
        layoutRow({16,17,18,19,20,21,22,23,24,25}, area, startY + keySize + spacing, keySize, spacing, 20);
        layoutRow({30,31,32,33,34,35,36,37,38}, area, startY + 2 * (keySize + spacing), keySize, spacing, 40);
        layoutRow({44,45,46,47,48,49,50}, area, startY + 3 * (keySize + spacing), keySize, spacing, 60);
    }
    
    void layoutRow(const std::vector<int>& scancodes, juce::Rectangle<int> area, int y, int keySize, int spacing, int offsetX = 0) {
        int x = area.getX() + offsetX;
        for (int scancode : scancodes) {
            if (keyButtons.count(scancode)) {
                keyButtons[scancode]->setBounds(x, y, keySize, keySize);
                x += keySize + spacing;
            }
        }
    }
    
    void setupSampleList() {
        addAndMakeVisible(sampleListBox);
        sampleListBox.setColour(juce::ListBox::backgroundColourId, Theme::background.darker(0.03f));
    }
    
    void setupSettingsPanel() {
        // 설정 UI는 나중에 추가
    }
    
    void layoutSettings(juce::Rectangle<int> area) {
        // 설정 레이아웃
    }
    
    void highlightKey(int scancode) {
        if (keyHighlights.count(scancode)) {
            keyHighlights[scancode] = 1.0f;
        }
    }
    
    int getScancodeFromKeyCode(int keyCode) {
        // JUCE KeyPress 코드를 scancode로 변환
        // 이는 간단한 매핑입니다
        if (keyCode >= 'A' && keyCode <= 'Z') {
            // QWERTY layout based
            const char* qwerty = "QWERTYUIOPASDFGHJKLZXCVBNM";
            const int scancodes[] = {
                16,17,18,19,20,21,22,23,24,25, // QWERTYUIOP
                30,31,32,33,34,35,36,37,38,    // ASDFGHJKL
                44,45,46,47,48,49,50           // ZXCVBNM
            };
            for (int i = 0; i < 26; ++i) {
                if (qwerty[i] == (char)keyCode) {
                    return scancodes[i];
                }
            }
        } else if (keyCode >= '0' && keyCode <= '9') {
            if (keyCode == '0') return 11;
            return 1 + (keyCode - '0'); // Number keys
        }
        return -1;
    }
};

} // namespace FXBoard
