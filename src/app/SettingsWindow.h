#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../audio/AudioEngine.h"
#include "ConfigManager.h"

namespace FXBoard {

/**
 * 키 매핑 설정 컴포넌트
 */
class KeyMappingPanel : public juce::Component {
public:
    KeyMappingPanel(AudioEngine* engine, ConfigManager* config);
    ~KeyMappingPanel() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void refreshMappings();
    void saveChanges();
    
private:
    struct MappingRow : public juce::Component {
        int scancode;
        juce::Label keyLabel;
        juce::ComboBox sampleCombo;
        juce::TextButton removeButton;
        
        std::function<void()> onRemove;
        std::function<void(int, const juce::String&)> onChange;
        
        MappingRow(int sc) : scancode(sc) {
            keyLabel.setText(getKeyName(sc), juce::dontSendNotification);
            keyLabel.setJustificationType(juce::Justification::centred);
            addAndMakeVisible(keyLabel);
            
            addAndMakeVisible(sampleCombo);
            sampleCombo.onChange = [this]() {
                if (onChange)
                    onChange(scancode, sampleCombo.getText());
            };
            
            removeButton.setButtonText("X");
            removeButton.onClick = [this]() {
                if (onRemove) onRemove();
            };
            addAndMakeVisible(removeButton);
        }
        
        void resized() override {
            auto bounds = getLocalBounds().reduced(5);
            keyLabel.setBounds(bounds.removeFromLeft(80));
            removeButton.setBounds(bounds.removeFromRight(40));
            sampleCombo.setBounds(bounds.reduced(5, 0));
        }
        
        static juce::String getKeyName(int scancode) {
            static std::map<int, juce::String> keyNames = {
                {16, "Q"}, {17, "W"}, {18, "E"}, {19, "R"}, {20, "T"},
                {21, "Y"}, {22, "U"}, {23, "I"}, {24, "O"}, {25, "P"},
                {30, "A"}, {31, "S"}, {32, "D"}, {33, "F"}, {34, "G"},
                {35, "H"}, {36, "J"}, {37, "K"}, {38, "L"},
                {44, "Z"}, {45, "X"}, {46, "C"}, {47, "V"}, {48, "B"},
                {49, "N"}, {50, "M"},
                {2, "1"}, {3, "2"}, {4, "3"}, {5, "4"}, {6, "5"},
                {7, "6"}, {8, "7"}, {9, "8"}, {10, "9"}, {11, "0"}
            };
            auto it = keyNames.find(scancode);
            return it != keyNames.end() ? it->second : juce::String(scancode);
        }
    };
    
    AudioEngine* audioEngine;
    ConfigManager* configManager;
    
    juce::Viewport viewport;
    juce::Component mappingContainer;
    juce::OwnedArray<MappingRow> mappingRows;
    
    juce::TextButton addButton;
    juce::ComboBox newKeyCombo;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyMappingPanel)
};

/**
 * 샘플 설정 컴포넌트
 */
class SampleSettingsPanel : public juce::Component {
public:
    SampleSettingsPanel(AudioEngine* engine, ConfigManager* config);
    ~SampleSettingsPanel() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void refreshSamples();
    void saveChanges();
    
private:
    struct SampleRow : public juce::Component {
        juce::String sampleId;
        juce::Label nameLabel;
        juce::Slider gainSlider;
        juce::Label gainLabel;
        juce::TextButton browseButton;
        juce::Label fileLabel;
        
        std::function<void(const juce::String&, float)> onGainChange;
        std::function<void(const juce::String&, const juce::File&)> onFileChange;
        
        SampleRow(const juce::String& id) : sampleId(id) {
            nameLabel.setText(id, juce::dontSendNotification);
            nameLabel.setFont(juce::Font(14.0f, juce::Font::bold));
            addAndMakeVisible(nameLabel);
            
            gainSlider.setRange(0.0, 2.0, 0.01);
            gainSlider.setValue(1.0);
            gainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
            gainSlider.onValueChange = [this]() {
                if (onGainChange)
                    onGainChange(sampleId, (float)gainSlider.getValue());
            };
            addAndMakeVisible(gainSlider);
            
            gainLabel.setText("Gain:", juce::dontSendNotification);
            addAndMakeVisible(gainLabel);
            
            browseButton.setButtonText("Browse...");
            browseButton.onClick = [this]() { browseSampleFile(); };
            addAndMakeVisible(browseButton);
            
            fileLabel.setFont(juce::Font(11.0f));
            fileLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
            addAndMakeVisible(fileLabel);
        }
        
        void resized() override {
            auto bounds = getLocalBounds().reduced(5);
            nameLabel.setBounds(bounds.removeFromTop(20));
            
            auto row = bounds.removeFromTop(30);
            gainLabel.setBounds(row.removeFromLeft(50));
            gainSlider.setBounds(row.removeFromLeft(200));
            browseButton.setBounds(row.removeFromRight(100).reduced(5, 0));
            
            fileLabel.setBounds(bounds.removeFromTop(20));
        }
        
        void browseSampleFile() {
            juce::FileChooser chooser("Select Sample File", juce::File(), "*.wav;*.mp3;*.ogg");
            chooser.launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                [this](const juce::FileChooser& fc) {
                    auto file = fc.getResult();
                    if (file.existsAsFile()) {
                        fileLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                        if (onFileChange)
                            onFileChange(sampleId, file);
                    }
                });
        }
    };
    
    AudioEngine* audioEngine;
    ConfigManager* configManager;
    
    juce::Viewport viewport;
    juce::Component sampleContainer;
    juce::OwnedArray<SampleRow> sampleRows;
    
    juce::TextButton addSampleButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleSettingsPanel)
};

/**
 * 오디오 설정 컴포넌트
 */
class AudioSettingsPanel : public juce::Component {
public:
    AudioSettingsPanel(AudioEngine* engine);
    ~AudioSettingsPanel() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    AudioEngine* audioEngine;
    
    juce::Label deviceLabel;
    juce::ComboBox deviceCombo;
    
    juce::Label sampleRateLabel;
    juce::ComboBox sampleRateCombo;
    
    juce::Label bufferSizeLabel;
    juce::ComboBox bufferSizeCombo;
    
    juce::TextButton applyButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSettingsPanel)
};

/**
 * 설정 창
 */
class SettingsWindow : public juce::DocumentWindow {
public:
    SettingsWindow(AudioEngine* engine, ConfigManager* config);
    ~SettingsWindow() override = default;
    
    void closeButtonPressed() override;
    
private:
    class SettingsComponent : public juce::TabbedComponent {
    public:
        SettingsComponent(AudioEngine* engine, ConfigManager* config);
        ~SettingsComponent() override = default;
        
        void saveAllSettings();
        
    private:
        AudioEngine* audioEngine;
        ConfigManager* configManager;
        
        KeyMappingPanel* keyMappingPanel;
        SampleSettingsPanel* sampleSettingsPanel;
        AudioSettingsPanel* audioSettingsPanel;
    };
    
    std::unique_ptr<SettingsComponent> settingsComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsWindow)
};

} // namespace FXBoard
