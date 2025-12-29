#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include "../audio/AudioEngine.h"

namespace FXBoard {

/**
 * 리듬게임 키 설정 UI
 * - 키보드 레이아웃 시각화
 * - 드래그 앤 드롭으로 샘플 할당
 * - 실시간 키 테스트
 */
class RhythmGameUI : public juce::Component,
                     public juce::KeyListener,
                     public juce::Timer {
public:
    RhythmGameUI(AudioEngine* engine) : audioEngine(engine) {
        setSize(800, 600);
        
        // 키 버튼 생성
        setupKeyButtons();
        
        // 샘플 리스트 생성
        setupSampleList();
        
        // 설정 패널
        setupSettingsPanel();
        
        // 타이머 시작 (키 시각화용)
        startTimer(16); // 60fps
    }
    
    ~RhythmGameUI() override {
        stopTimer();
    }
    
    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colours::darkgrey);
        
        // 타이틀
        g.setColour(juce::Colours::white);
        g.setFont(24.0f);
        g.drawText("FXBoard - Rhythm Game Setup", 
                   getLocalBounds().removeFromTop(50), 
                   juce::Justification::centred);
        
        // 레이턴시 표시
        g.setFont(14.0f);
        juce::String latencyText = "Latency: " + 
            juce::String(audioEngine->getLatencyMs(), 2) + " ms";
        g.drawText(latencyText, 10, 30, 200, 20, juce::Justification::left);
        
        // CPU 사용률
        juce::String cpuText = "CPU: " + 
            juce::String(audioEngine->getCpuLoad(), 1) + "%";
        g.drawText(cpuText, 10, 50, 200, 20, juce::Justification::left);
    }
    
    void resized() override {
        auto bounds = getLocalBounds();
        bounds.removeFromTop(80); // 헤더 영역
        
        // 왼쪽: 키보드 레이아웃
        auto keyboardArea = bounds.removeFromLeft(500).reduced(10);
        layoutKeyButtons(keyboardArea);
        
        // 오른쪽 상단: 샘플 리스트
        auto rightArea = bounds.reduced(10);
        sampleListBox.setBounds(rightArea.removeFromTop(300));
        
        // 오른쪽 하단: 설정
        settingsArea = rightArea.removeFromTop(200);
        layoutSettings(settingsArea);
    }
    
    bool keyPressed(const juce::KeyPress& key, juce::Component*) override {
        int keyCode = key.getKeyCode();
        int scancode = getScancodeFromKeyCode(keyCode);
        
        if (scancode >= 0) {
            // 키 시각화
            highlightKey(scancode);
            
            // 이벤트 푸시
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
        // Key up 처리
        return false;
    }
    
    void timerCallback() override {
        // 키 하이라이트 페이드아웃
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
    
    // 키 버튼들
    struct KeyButton : public juce::TextButton {
        int scancode;
        float highlight = 0.0f;
        juce::String assignedSample;
        
        void paint(juce::Graphics& g) override {
            auto bounds = getLocalBounds().toFloat();
            
            // 배경
            juce::Colour bgColor = juce::Colours::darkgrey;
            if (highlight > 0.0f) {
                bgColor = juce::Colours::yellow.withAlpha(highlight);
            }
            g.setColour(bgColor);
            g.fillRoundedRectangle(bounds, 5.0f);
            
            // 테두리
            g.setColour(juce::Colours::white);
            g.drawRoundedRectangle(bounds.reduced(1.0f), 5.0f, 2.0f);
            
            // 키 이름
            g.setFont(16.0f);
            g.drawText(getButtonText(), 
                      bounds.reduced(5.0f).removeFromTop(20.0f), 
                      juce::Justification::centred);
            
            // 할당된 샘플
            if (assignedSample.isNotEmpty()) {
                g.setFont(12.0f);
                g.setColour(juce::Colours::lightgreen);
                g.drawText(assignedSample, 
                          bounds.reduced(5.0f), 
                          juce::Justification::centredBottom);
            }
        }
    };
    
    std::map<int, std::unique_ptr<KeyButton>> keyButtons;
    std::map<int, float> keyHighlights;
    
    juce::ListBox sampleListBox;
    juce::Rectangle<int> settingsArea;
    
    void setupKeyButtons() {
        // QWERTY 레이아웃 기준
        // 숫자 행: 1234567890
        createKeyRow({2,3,4,5,6,7,8,9,10,11}, {"1","2","3","4","5","6","7","8","9","0"});
        
        // 상단 행: QWERTYUIOP
        createKeyRow({16,17,18,19,20,21,22,23,24,25}, {"Q","W","E","R","T","Y","U","I","O","P"});
        
        // 중간 행: ASDFGHJKL
        createKeyRow({30,31,32,33,34,35,36,37,38}, {"A","S","D","F","G","H","J","K","L"});
        
        // 하단 행: ZXCVBNM
        createKeyRow({44,45,46,47,48,49,50}, {"Z","X","C","V","B","N","M"});
    }
    
    void createKeyRow(const std::vector<int>& scancodes, 
                     const std::vector<juce::String>& labels) {
        for (size_t i = 0; i < scancodes.size(); ++i) {
            auto button = std::make_unique<KeyButton>();
            button->scancode = scancodes[i];
            button->setButtonText(labels[i]);
            addAndMakeVisible(button.get());
            keyButtons[scancodes[i]] = std::move(button);
            keyHighlights[scancodes[i]] = 0.0f;
        }
    }
    
    void layoutKeyButtons(juce::Rectangle<int> area) {
        int keySize = 50;
        int spacing = 5;
        int startY = 10;
        
        // 숫자 행
        layoutRow({2,3,4,5,6,7,8,9,10,11}, area, startY, keySize, spacing);
        
        // QWERTY 행
        layoutRow({16,17,18,19,20,21,22,23,24,25}, area, startY + 60, keySize, spacing);
        
        // ASDF 행
        layoutRow({30,31,32,33,34,35,36,37,38}, area, startY + 120, keySize, spacing, 30);
        
        // ZXCV 행
        layoutRow({44,45,46,47,48,49,50}, area, startY + 180, keySize, spacing, 60);
    }
    
    void layoutRow(const std::vector<int>& scancodes, 
                   juce::Rectangle<int> area,
                   int y, int keySize, int spacing, int offsetX = 0) {
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
            // QWERTY 레이아웃 기준
            const char* qwerty = "QWERTYUIOPASDFGHJKLZXCVBNM";
            const int scancodes[] = {
                16,17,18,19,20,21,22,23,24,25, // QWERTYUIOP
                30,31,32,33,34,35,36,37,38,    // ASDFGHJKL
                44,45,46,47,48,49,50           // ZXCVBNM
            };
            for (int i = 0; i < 26; ++i) {
                if (qwerty[i] == keyCode) {
                    return scancodes[i];
                }
            }
        } else if (keyCode >= '0' && keyCode <= '9') {
            return 2 + (keyCode - '0'); // 숫자 키
        }
        return -1;
    }
};

} // namespace FXBoard
