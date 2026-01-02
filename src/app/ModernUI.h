#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../audio/AudioEngine.h"

namespace FXBoard {

/**
 * 모던한 색상 테마
 */
namespace UITheme {
    static const juce::Colour background = juce::Colour(0xff1e1e1e);
    static const juce::Colour surface = juce::Colour(0xff2d2d2d);
    static const juce::Colour surfaceLight = juce::Colour(0xff3d3d3d);
    static const juce::Colour primary = juce::Colour(0xff00d4ff);
    static const juce::Colour secondary = juce::Colour(0xffff6b35);
    static const juce::Colour accent = juce::Colour(0xff7c4dff);
    static const juce::Colour success = juce::Colour(0xff4caf50);
    static const juce::Colour warning = juce::Colour(0xffffc107);
    static const juce::Colour error = juce::Colour(0xfff44336);
    static const juce::Colour textPrimary = juce::Colour(0xffffffff);
    static const juce::Colour textSecondary = juce::Colour(0xffb0b0b0);
    static const juce::Colour border = juce::Colour(0xff404040);
}

/**
 * 키 버튼 컴포넌트
 */
class KeyButton : public juce::Component {
public:
    KeyButton(const juce::String& keyName, const juce::String& sampleName, int sc)
        : key(keyName), sample(sampleName), scancode(sc), highlight(0.0f), active(false) {}
    
    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();
        
        // 그림자
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.fillRoundedRectangle(bounds.translated(0, 3), 8.0f);
        
        // 배경
        juce::Colour bgColor = active ? UITheme::primary : UITheme::surface;
        if (highlight > 0.0f) {
            bgColor = bgColor.interpolatedWith(UITheme::accent, highlight);
        }
        g.setColour(bgColor);
        g.fillRoundedRectangle(bounds, 8.0f);
        
        // 테두리
        g.setColour(UITheme::border.brighter(highlight * 0.5f));
        g.drawRoundedRectangle(bounds.reduced(1.0f), 8.0f, 2.0f);
        
        // 키 이름
        g.setColour(UITheme::textPrimary);
        g.setFont(juce::Font(20.0f, juce::Font::bold));
        g.drawText(key, bounds.reduced(8.0f).removeFromTop(30), juce::Justification::centred);
        
        // 샘플 이름
        if (sample.isNotEmpty()) {
            g.setColour(UITheme::textSecondary);
            g.setFont(juce::Font(12.0f));
            g.drawText(sample, bounds.reduced(8.0f), juce::Justification::centredBottom);
        }
    }
    
    void setHighlight(float value) {
        highlight = juce::jlimit(0.0f, 1.0f, value);
        repaint();
    }
    
    void setActive(bool isActive) {
        active = isActive;
        repaint();
    }
    
    void setSample(const juce::String& sampleName) {
        sample = sampleName;
        repaint();
    }
    
    int getScancode() const { return scancode; }
    
private:
    juce::String key;
    juce::String sample;
    int scancode;
    float highlight;
    bool active;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyButton)
};

/**
 * 상태 표시 패널
 */
class StatusPanel : public juce::Component, public juce::Timer {
public:
    StatusPanel(AudioEngine* engine) : audioEngine(engine) {
        startTimer(100);
    }
    
    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();
        
        // 배경
        g.setColour(UITheme::surface);
        g.fillRoundedRectangle(bounds, 6.0f);
        
        // 레이턴시
        auto latencyArea = bounds.removeFromLeft(bounds.getWidth() / 3.0f).reduced(10.0f);
        double latency = audioEngine->getLatencyMs();
        g.setColour(latency < 10.0 ? UITheme::success : UITheme::warning);
        g.setFont(juce::Font(16.0f, juce::Font::bold));
        g.drawText(juce::String(latency, 1) + " ms", latencyArea, juce::Justification::centred);
        g.setColour(UITheme::textSecondary);
        g.setFont(juce::Font(10.0f));
        g.drawText("Latency", latencyArea.translated(0, 20), juce::Justification::centred);
        
        // CPU
        auto cpuArea = bounds.removeFromLeft(bounds.getWidth() / 2.0f).reduced(10.0f);
        double cpu = audioEngine->getCpuLoad();
        g.setColour(cpu < 50.0 ? UITheme::success : (cpu < 80.0 ? UITheme::warning : UITheme::error));
        g.setFont(juce::Font(16.0f, juce::Font::bold));
        g.drawText(juce::String(cpu, 1) + "%", cpuArea, juce::Justification::centred);
        g.setColour(UITheme::textSecondary);
        g.setFont(juce::Font(10.0f));
        g.drawText("CPU Load", cpuArea.translated(0, 20), juce::Justification::centred);
        
        // 샘플 수
        auto samplesArea = bounds.reduced(10.0f);
        int numSamples = audioEngine->getSampleManager().getNumSamples();
        g.setColour(UITheme::primary);
        g.setFont(juce::Font(16.0f, juce::Font::bold));
        g.drawText(juce::String(numSamples), samplesArea, juce::Justification::centred);
        g.setColour(UITheme::textSecondary);
        g.setFont(juce::Font(10.0f));
        g.drawText("Samples", samplesArea.translated(0, 20), juce::Justification::centred);
    }
    
    void timerCallback() override {
        repaint();
    }
    
private:
    AudioEngine* audioEngine;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StatusPanel)
};

/**
 * 키보드 레이아웃 패널
 */
class KeyboardPanel : public juce::Component, public juce::KeyListener, public juce::Timer {
public:
    KeyboardPanel(AudioEngine* engine) : audioEngine(engine) {
        setupKeyButtons();
        startTimer(16); // 60fps
        
        // 키보드 포커스 설정
        setWantsKeyboardFocus(true);
        addKeyListener(this);
    }
    
    ~KeyboardPanel() override {
        stopTimer();
        removeKeyListener(this);
    }
    
    void paint(juce::Graphics& g) override {
        g.fillAll(UITheme::background);
        
        // 타이틀
        g.setColour(UITheme::textPrimary);
        g.setFont(juce::Font(24.0f, juce::Font::bold));
        g.drawText("Keyboard Mapping", getLocalBounds().removeFromTop(50).reduced(20, 10),
                   juce::Justification::centredLeft);
    }
    
    void resized() override {
        auto bounds = getLocalBounds().reduced(20);
        bounds.removeFromTop(60); // 타이틀 영역
        
        layoutKeyRow({16,17,18,19,20,21,22,23,24,25}, bounds.removeFromTop(70), 0);
        bounds.removeFromTop(10);
        layoutKeyRow({30,31,32,33,34,35,36,37,38}, bounds.removeFromTop(70), 30);
        bounds.removeFromTop(10);
        layoutKeyRow({44,45,46,47,48,49,50}, bounds.removeFromTop(70), 60);
    }
    
    void visibilityChanged() override {
        if (isVisible()) {
            grabKeyboardFocus();
            updateMappings();
        }
    }
    
    bool keyPressed(const juce::KeyPress& key, juce::Component*) override {
        int keyCode = key.getKeyCode();
        int scancode = getScancodeFromKeyCode(keyCode);
        
        if (scancode >= 0 && keyButtons.count(scancode)) {
            keyButtons[scancode]->setHighlight(1.0f);
            keyButtons[scancode]->getProperties().set("highlight", 1.0f);
            
            KeyEvent event;
            event.type = KeyEvent::Down;
            event.scancode = static_cast<uint32_t>(scancode);
            event.timestampNs = static_cast<uint64_t>(juce::Time::getHighResolutionTicks());
            audioEngine->pushEvent(event);
            
            juce::Logger::writeToLog("Key pressed: " + juce::String(keyCode) + 
                                     " -> scancode: " + juce::String(scancode));
            
            return true;
        }
        return false;
    }
    
    bool keyStateChanged(bool, juce::Component*) override {
        return false;
    }
    
    void timerCallback() override {
        for (auto& [scancode, button] : keyButtons) {
            float currentHighlight = button->getProperties()["highlight"];
            if (currentHighlight > 0.0f) {
                currentHighlight -= 0.05f;
                if (currentHighlight < 0.0f) currentHighlight = 0.0f;
                button->getProperties().set("highlight", currentHighlight);
                button->setHighlight(currentHighlight);
            }
        }
    }
    
    void updateMappings() {
        auto mappings = audioEngine->getKeyMappings();
        for (auto& [scancode, button] : keyButtons) {
            uint32_t sc = static_cast<uint32_t>(scancode);
            if (mappings.count(sc)) {
                button->setSample(mappings[sc]);
                button->setActive(true);
            } else {
                button->setSample("");
                button->setActive(false);
            }
        }
    }
    
private:
    AudioEngine* audioEngine;
    std::map<int, std::unique_ptr<KeyButton>> keyButtons;
    
    void setupKeyButtons() {
        createKeyRow({16,17,18,19,20,21,22,23,24,25}, {"Q","W","E","R","T","Y","U","I","O","P"});
        createKeyRow({30,31,32,33,34,35,36,37,38}, {"A","S","D","F","G","H","J","K","L"});
        createKeyRow({44,45,46,47,48,49,50}, {"Z","X","C","V","B","N","M"});
        
        updateMappings();
    }
    
    void createKeyRow(const std::vector<int>& scancodes, const std::vector<juce::String>& labels) {
        for (size_t i = 0; i < scancodes.size(); ++i) {
            auto button = std::make_unique<KeyButton>(labels[i], "", scancodes[i]);
            button->getProperties().set("highlight", 0.0f);
            addAndMakeVisible(button.get());
            keyButtons[scancodes[i]] = std::move(button);
        }
    }
    
    void layoutKeyRow(const std::vector<int>& scancodes, juce::Rectangle<int> area, int offsetX) {
        int keySize = 60;
        int spacing = 8;
        int x = area.getX() + offsetX;
        int y = area.getY();
        
        for (int scancode : scancodes) {
            if (keyButtons.count(scancode)) {
                keyButtons[scancode]->setBounds(x, y, keySize, keySize);
                x += keySize + spacing;
            }
        }
    }
    
    int getScancodeFromKeyCode(int keyCode) {
        // 소문자를 대문자로 변환
        if (keyCode >= 'a' && keyCode <= 'z') {
            keyCode = keyCode - 'a' + 'A';
        }
        
        // 알파벳 키
        if (keyCode >= 'A' && keyCode <= 'Z') {
            const char* qwerty = "QWERTYUIOPASDFGHJKLZXCVBNM";
            const int scancodes[] = {
                16,17,18,19,20,21,22,23,24,25,
                30,31,32,33,34,35,36,37,38,
                44,45,46,47,48,49,50
            };
            for (int i = 0; i < 26; ++i) {
                if (qwerty[i] == (char)keyCode) {
                    return scancodes[i];
                }
            }
        }
        
        // 특수 키 처리
        if (keyCode == ';' || keyCode == ':') {
            return 39;  // Semicolon scancode
        }
        if (keyCode == ' ' || keyCode == 32) {  // Space (ASCII 32)
            return 57;
        }
        
        // Alt 키는 modifier로 처리되므로 별도 확인 필요
        return -1;
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyboardPanel)
};

/**
 * 샘플 리스트 모델
 */
class SampleListModel : public juce::ListBoxModel {
public:
    SampleListModel(AudioEngine* engine) : audioEngine(engine) {
        refresh();
    }
    
    void refresh() {
        samples = audioEngine->getSampleManager().getAllSampleIds();
    }
    
    int getNumRows() override {
        return samples.size();
    }
    
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override {
        if (rowNumber >= samples.size()) return;
        
        auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat();
        
        // 배경
        if (rowIsSelected) {
            g.setColour(UITheme::primary.withAlpha(0.3f));
            g.fillRect(bounds);
        } else if (rowNumber % 2 == 0) {
            g.setColour(UITheme::surface);
            g.fillRect(bounds);
        }
        
        // 샘플 이름
        g.setColour(UITheme::textPrimary);
        g.setFont(juce::Font(14.0f));
        g.drawText(samples[rowNumber], bounds.reduced(10, 5), juce::Justification::centredLeft);
    }
    
private:
    AudioEngine* audioEngine;
    juce::StringArray samples;
};

/**
 * 샘플 패널
 */
class SamplesPanel : public juce::Component {
public:
    SamplesPanel(AudioEngine* engine) 
        : audioEngine(engine), 
          listModel(engine),
          addButton("Add Sample"),
          refreshButton("Refresh") {
        
        addAndMakeVisible(listBox);
        listBox.setModel(&listModel);
        listBox.setColour(juce::ListBox::backgroundColourId, UITheme::surface);
        listBox.setRowHeight(40);
        
        addButton.setColour(juce::TextButton::buttonColourId, UITheme::primary);
        addButton.onClick = [this]() { /* Add sample logic */ };
        addAndMakeVisible(addButton);
        
        refreshButton.setColour(juce::TextButton::buttonColourId, UITheme::secondary);
        refreshButton.onClick = [this]() { listModel.refresh(); listBox.repaint(); };
        addAndMakeVisible(refreshButton);
    }
    
    void paint(juce::Graphics& g) override {
        g.fillAll(UITheme::background);
        
        g.setColour(UITheme::textPrimary);
        g.setFont(juce::Font(24.0f, juce::Font::bold));
        g.drawText("Samples", getLocalBounds().removeFromTop(50).reduced(20, 10),
                   juce::Justification::centredLeft);
    }
    
    void resized() override {
        auto bounds = getLocalBounds().reduced(20);
        bounds.removeFromTop(60);
        
        auto buttonArea = bounds.removeFromBottom(50);
        buttonArea.removeFromTop(10);
        addButton.setBounds(buttonArea.removeFromLeft(120));
        buttonArea.removeFromLeft(10);
        refreshButton.setBounds(buttonArea.removeFromLeft(120));
        
        bounds.removeFromBottom(10);
        listBox.setBounds(bounds);
    }
    
private:
    AudioEngine* audioEngine;
    SampleListModel listModel;
    juce::ListBox listBox;
    juce::TextButton addButton;
    juce::TextButton refreshButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SamplesPanel)
};

/**
 * 메인 UI 컴포넌트
 */
class ModernMainComponent : public juce::Component, public juce::KeyListener, private juce::Timer {
public:
    ModernMainComponent(AudioEngine* engine) 
        : audioEngine(engine),
          tabbedComponent(juce::TabbedButtonBar::TabsAtTop),
          statusPanel(engine),
          keyboardPanel(engine),
          samplesPanel(engine),
          settingsButton("Settings") {
        
        // 상태 패널
        addAndMakeVisible(statusPanel);
        
        // 탭 컴포넌트
        tabbedComponent.setTabBarDepth(40);
        tabbedComponent.setColour(juce::TabbedComponent::backgroundColourId, UITheme::background);
        tabbedComponent.setColour(juce::TabbedComponent::outlineColourId, UITheme::border);
        
        tabbedComponent.addTab("Keyboard", UITheme::surface, &keyboardPanel, false);
        tabbedComponent.addTab("Samples", UITheme::surface, &samplesPanel, false);
        
        addAndMakeVisible(tabbedComponent);
        
        // 설정 버튼
        settingsButton.setColour(juce::TextButton::buttonColourId, UITheme::accent);
        addAndMakeVisible(settingsButton);
        
        // 키보드 리스너 설정
        setWantsKeyboardFocus(true);
        addKeyListener(this);
        
        // 타이머 시작 (키 매핑 업데이트용)
        startTimer(1000);
        
        setSize(1000, 700);
    }
    
    ~ModernMainComponent() override {
        stopTimer();
        removeKeyListener(this);
    }
    
    void paint(juce::Graphics& g) override {
        g.fillAll(UITheme::background);
        
        // 헤더
        auto headerBounds = getLocalBounds().removeFromTop(80).toFloat();
        g.setColour(UITheme::surface);
        g.fillRect(headerBounds);
        
        // 타이틀
        g.setColour(UITheme::textPrimary);
        g.setFont(juce::Font(32.0f, juce::Font::bold));
        g.drawText("FXBoard", headerBounds.reduced(20, 10).removeFromLeft(200), 
                   juce::Justification::centredLeft);
    }
    
    void resized() override {
        auto bounds = getLocalBounds();
        
        // 헤더
        auto headerArea = bounds.removeFromTop(80);
        headerArea.removeFromLeft(220);
        headerArea.removeFromRight(20);
        settingsButton.setBounds(headerArea.removeFromRight(100).reduced(0, 20));
        
        // 상태 패널
        auto statusArea = bounds.removeFromTop(80);
        statusPanel.setBounds(statusArea.reduced(20, 10));
        
        // 탭 컴포넌트
        tabbedComponent.setBounds(bounds.reduced(10));
    }
    
    void visibilityChanged() override {
        if (isVisible()) {
            // 키보드 탭에 포커스
            keyboardPanel.grabKeyboardFocus();
            keyboardPanel.updateMappings();
        }
    }
    
    void timerCallback() override {
        // 주기적으로 키 매핑 업데이트
        if (tabbedComponent.getCurrentTabIndex() == 0) {
            keyboardPanel.updateMappings();
        }
    }
    
    bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override {
        juce::Logger::writeToLog("ModernMainComponent keyPressed: " + juce::String(key.getKeyCode()));
        return keyboardPanel.keyPressed(key, originatingComponent);
    }
    
    bool keyStateChanged(bool isKeyDown, juce::Component* originatingComponent) override {
        return keyboardPanel.keyStateChanged(isKeyDown, originatingComponent);
    }
    
private:
    AudioEngine* audioEngine;
    juce::TabbedComponent tabbedComponent;
    StatusPanel statusPanel;
    KeyboardPanel keyboardPanel;
    SamplesPanel samplesPanel;
    juce::TextButton settingsButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModernMainComponent)
};

} // namespace FXBoard
