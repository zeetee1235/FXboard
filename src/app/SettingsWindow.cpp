#include "SettingsWindow.h"

namespace FXBoard {

// ==============================================================================
// KeyMappingPanel
// ==============================================================================

KeyMappingPanel::KeyMappingPanel(AudioEngine* engine, ConfigManager* config)
    : audioEngine(engine), configManager(config) {
    
    addAndMakeVisible(viewport);
    viewport.setViewedComponent(&mappingContainer, false);
    
    addButton.setButtonText("Add Key Mapping");
    addButton.onClick = [this]() {
        int scancode = newKeyCombo.getSelectedId();
        if (scancode > 0) {
            auto* row = mappingRows.add(new MappingRow(scancode));
            
            // Populate sample combo
            auto samples = audioEngine->getSampleManager().getAllSampleIds();
            for (const auto& sample : samples) {
                row->sampleCombo.addItem(sample, row->sampleCombo.getNumItems() + 1);
            }
            
            row->onChange = [this](int sc, const juce::String& sample) {
                audioEngine->mapKeyToSample(sc, sample);
            };
            
            row->onRemove = [this, row]() {
                audioEngine->unmapKey(row->scancode);
                mappingRows.removeObject(row);
                resized();
            };
            
            mappingContainer.addAndMakeVisible(row);
            resized();
        }
    };
    addAndMakeVisible(addButton);
    
    newKeyCombo.addItem("Q (16)", 16);
    newKeyCombo.addItem("W (17)", 17);
    newKeyCombo.addItem("E (18)", 18);
    newKeyCombo.addItem("A (30)", 30);
    newKeyCombo.addItem("S (31)", 31);
    newKeyCombo.addItem("D (32)", 32);
    newKeyCombo.addItem("Z (44)", 44);
    newKeyCombo.addItem("X (45)", 45);
    newKeyCombo.addItem("C (46)", 46);
    addAndMakeVisible(newKeyCombo);
    
    refreshMappings();
}

void KeyMappingPanel::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xfff5f5f5));
    
    g.setColour(juce::Colours::darkgrey);
    g.setFont(16.0f);
    g.drawText("Key Mappings", getLocalBounds().removeFromTop(40).reduced(10), 
               juce::Justification::centredLeft);
}

void KeyMappingPanel::resized() {
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(40);
    
    auto bottom = bounds.removeFromBottom(40);
    newKeyCombo.setBounds(bottom.removeFromLeft(200).reduced(5));
    addButton.setBounds(bottom.removeFromLeft(150).reduced(5));
    
    viewport.setBounds(bounds);
    
    int rowHeight = 50;
    mappingContainer.setSize(viewport.getWidth() - 20, mappingRows.size() * rowHeight);
    
    int y = 0;
    for (auto* row : mappingRows) {
        row->setBounds(0, y, mappingContainer.getWidth(), rowHeight);
        y += rowHeight;
    }
}

void KeyMappingPanel::refreshMappings() {
    mappingRows.clear();
    
    auto mappings = audioEngine->getKeyMappings();
    auto samples = audioEngine->getSampleManager().getAllSampleIds();
    
    for (const auto& [scancode, sampleId] : mappings) {
        auto* row = mappingRows.add(new MappingRow(scancode));
        
        for (const auto& sample : samples) {
            row->sampleCombo.addItem(sample, row->sampleCombo.getNumItems() + 1);
        }
        
        int idx = samples.indexOf(sampleId);
        if (idx >= 0) {
            row->sampleCombo.setSelectedItemIndex(idx);
        }
        
        row->onChange = [this](int sc, const juce::String& sample) {
            audioEngine->mapKeyToSample(sc, sample);
        };
        
        row->onRemove = [this, row]() {
            audioEngine->unmapKey(row->scancode);
            mappingRows.removeObject(row);
            resized();
        };
        
        mappingContainer.addAndMakeVisible(row);
    }
    
    resized();
}

void KeyMappingPanel::saveChanges() {
    // Save to config
    if (configManager) {
        configManager->saveConfig(juce::File::getCurrentWorkingDirectory().getChildFile("config.json"));
    }
}

// ==============================================================================
// SampleSettingsPanel
// ==============================================================================

SampleSettingsPanel::SampleSettingsPanel(AudioEngine* engine, ConfigManager* config)
    : audioEngine(engine), configManager(config) {
    
    addAndMakeVisible(viewport);
    viewport.setViewedComponent(&sampleContainer, false);
    
    addSampleButton.setButtonText("Add New Sample");
    addSampleButton.onClick = [this]() {
        // Simple input dialog
        auto name = juce::String("sample") + juce::String(sampleRows.size() + 1);
        auto* row = sampleRows.add(new SampleRow(name));
        
        row->onGainChange = [this](const juce::String& id, float gain) {
            audioEngine->getSampleManager().setSampleGain(id, gain);
        };
        
        row->onFileChange = [this, row](const juce::String& id, const juce::File& file) {
            audioEngine->getSampleManager().loadSample(id, file);
        };
        
        sampleContainer.addAndMakeVisible(row);
        resized();
    };
    addAndMakeVisible(addSampleButton);
    
    refreshSamples();
}

void SampleSettingsPanel::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xfff5f5f5));
    
    g.setColour(juce::Colours::darkgrey);
    g.setFont(16.0f);
    g.drawText("Sample Settings", getLocalBounds().removeFromTop(40).reduced(10), 
               juce::Justification::centredLeft);
}

void SampleSettingsPanel::resized() {
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(40);
    
    auto bottom = bounds.removeFromBottom(40);
    addSampleButton.setBounds(bottom.reduced(5));
    
    viewport.setBounds(bounds);
    
    int rowHeight = 80;
    sampleContainer.setSize(viewport.getWidth() - 20, sampleRows.size() * rowHeight);
    
    int y = 0;
    for (auto* row : sampleRows) {
        row->setBounds(0, y, sampleContainer.getWidth(), rowHeight);
        y += rowHeight;
    }
}

void SampleSettingsPanel::refreshSamples() {
    sampleRows.clear();
    
    auto samples = audioEngine->getSampleManager().getAllSampleIds();
    
    for (const auto& sampleId : samples) {
        auto* row = sampleRows.add(new SampleRow(sampleId));
        
        row->gainSlider.setValue(audioEngine->getSampleManager().getSampleGain(sampleId));
        
        row->onGainChange = [this](const juce::String& id, float gain) {
            audioEngine->getSampleManager().setSampleGain(id, gain);
        };
        
        row->onFileChange = [this, row](const juce::String& id, const juce::File& file) {
            audioEngine->getSampleManager().loadSample(id, file);
            row->fileLabel.setText(file.getFullPathName(), juce::dontSendNotification);
        };
        
        sampleContainer.addAndMakeVisible(row);
    }
    
    resized();
}

void SampleSettingsPanel::saveChanges() {
    if (configManager) {
        configManager->saveConfig(juce::File::getCurrentWorkingDirectory().getChildFile("config.json"));
    }
}

// ==============================================================================
// AudioSettingsPanel
// ==============================================================================

AudioSettingsPanel::AudioSettingsPanel(AudioEngine* engine)
    : audioEngine(engine) {
    
    deviceLabel.setText("Audio Device:", juce::dontSendNotification);
    addAndMakeVisible(deviceLabel);
    
    // Populate device list
    auto& deviceManager = audioEngine->getDeviceManager();
    auto* deviceType = deviceManager.getCurrentDeviceTypeObject();
    if (deviceType) {
        auto deviceNames = deviceType->getDeviceNames();
        for (int i = 0; i < deviceNames.size(); ++i) {
            deviceCombo.addItem(deviceNames[i], i + 1);
        }
        
        auto* currentDevice = deviceManager.getCurrentAudioDevice();
        if (currentDevice) {
            deviceCombo.setText(currentDevice->getName(), juce::dontSendNotification);
        }
    }
    addAndMakeVisible(deviceCombo);
    
    sampleRateLabel.setText("Sample Rate:", juce::dontSendNotification);
    addAndMakeVisible(sampleRateLabel);
    
    sampleRateCombo.addItem("44100 Hz", 1);
    sampleRateCombo.addItem("48000 Hz", 2);
    sampleRateCombo.addItem("96000 Hz", 3);
    sampleRateCombo.setSelectedId(2);
    addAndMakeVisible(sampleRateCombo);
    
    bufferSizeLabel.setText("Buffer Size:", juce::dontSendNotification);
    addAndMakeVisible(bufferSizeLabel);
    
    bufferSizeCombo.addItem("64 samples", 1);
    bufferSizeCombo.addItem("128 samples", 2);
    bufferSizeCombo.addItem("256 samples", 3);
    bufferSizeCombo.addItem("512 samples", 4);
    bufferSizeCombo.setSelectedId(2);
    addAndMakeVisible(bufferSizeCombo);
    
    applyButton.setButtonText("Apply Changes");
    applyButton.onClick = [this]() {
        // Apply audio settings
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::InfoIcon,
            "Settings",
            "Audio settings will be applied on restart.",
            "OK"
        );
    };
    addAndMakeVisible(applyButton);
}

void AudioSettingsPanel::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xfff5f5f5));
    
    g.setColour(juce::Colours::darkgrey);
    g.setFont(16.0f);
    g.drawText("Audio Settings", getLocalBounds().removeFromTop(40).reduced(10), 
               juce::Justification::centredLeft);
}

void AudioSettingsPanel::resized() {
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(50);
    
    auto row1 = bounds.removeFromTop(40);
    deviceLabel.setBounds(row1.removeFromLeft(120));
    deviceCombo.setBounds(row1.reduced(5));
    
    bounds.removeFromTop(10);
    auto row2 = bounds.removeFromTop(40);
    sampleRateLabel.setBounds(row2.removeFromLeft(120));
    sampleRateCombo.setBounds(row2.reduced(5));
    
    bounds.removeFromTop(10);
    auto row3 = bounds.removeFromTop(40);
    bufferSizeLabel.setBounds(row3.removeFromLeft(120));
    bufferSizeCombo.setBounds(row3.reduced(5));
    
    bounds.removeFromTop(20);
    applyButton.setBounds(bounds.removeFromTop(40).reduced(5));
}

// ==============================================================================
// SettingsWindow::SettingsComponent
// ==============================================================================

SettingsWindow::SettingsComponent::SettingsComponent(AudioEngine* engine, ConfigManager* config)
    : juce::TabbedComponent(juce::TabbedButtonBar::TabsAtTop)
    , audioEngine(engine)
    , configManager(config) {
    
    keyMappingPanel = new KeyMappingPanel(engine, config);
    addTab("Key Mapping", juce::Colour(0xfff5f5f5), keyMappingPanel, true);
    
    sampleSettingsPanel = new SampleSettingsPanel(engine, config);
    addTab("Samples", juce::Colour(0xfff5f5f5), sampleSettingsPanel, true);
    
    audioSettingsPanel = new AudioSettingsPanel(engine);
    addTab("Audio", juce::Colour(0xfff5f5f5), audioSettingsPanel, true);
    
    setSize(800, 600);
}

void SettingsWindow::SettingsComponent::saveAllSettings() {
    keyMappingPanel->saveChanges();
    sampleSettingsPanel->saveChanges();
}

// ==============================================================================
// SettingsWindow
// ==============================================================================

SettingsWindow::SettingsWindow(AudioEngine* engine, ConfigManager* config)
    : DocumentWindow("FXBoard Settings",
                     juce::Desktop::getInstance().getDefaultLookAndFeel()
                         .findColour(juce::ResizableWindow::backgroundColourId),
                     DocumentWindow::allButtons) {
    
    setUsingNativeTitleBar(true);
    
    settingsComponent = std::make_unique<SettingsComponent>(engine, config);
    setContentOwned(settingsComponent.get(), true);
    
    setResizable(true, true);
    centreWithSize(800, 600);
    setVisible(true);
}

void SettingsWindow::closeButtonPressed() {
    settingsComponent->saveAllSettings();
    setVisible(false);
}

} // namespace FXBoard
