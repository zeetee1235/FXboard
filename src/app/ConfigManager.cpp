#include "ConfigManager.h"

namespace FXBoard {

ConfigManager::ConfigManager() 
    : config("FXBoardConfig") {
}

bool ConfigManager::loadConfig(const juce::File& configFile) {
    if (!configFile.existsAsFile()) {
        juce::Logger::writeToLog("Config file not found: " + configFile.getFullPathName());
        return false;
    }
    
    auto json = juce::JSON::parse(configFile);
    if (!json.isObject()) {
        juce::Logger::writeToLog("Invalid config file format");
        return false;
    }
    
    // JSON을 ValueTree로 변환
    config = juce::ValueTree("FXBoardConfig");
    
    // 기본 설정 파싱
    if (json.hasProperty("audio")) {
        auto audioTree = juce::ValueTree("Audio");
        auto* audioObj = json.getProperty("audio", juce::var()).getDynamicObject();
        if (audioObj != nullptr) {
            for (auto& prop : audioObj->getProperties()) {
                audioTree.setProperty(prop.name, prop.value, nullptr);
            }
        }
        config.appendChild(audioTree, nullptr);
    }
    
    if (json.hasProperty("keymapping")) {
        auto keyTree = juce::ValueTree("KeyMapping");
        auto* keyObj = json.getProperty("keymapping", juce::var()).getDynamicObject();
        if (keyObj != nullptr) {
            for (auto& prop : keyObj->getProperties()) {
                keyTree.setProperty(prop.name, prop.value, nullptr);
            }
        }
        config.appendChild(keyTree, nullptr);
    }
    
    juce::Logger::writeToLog("Config loaded from: " + configFile.getFullPathName());
    return true;
}

bool ConfigManager::saveConfig(const juce::File& configFile) {
    // ValueTree를 JSON으로 변환
    juce::DynamicObject::Ptr root = new juce::DynamicObject();
    
    for (int i = 0; i < config.getNumChildren(); ++i) {
        auto child = config.getChild(i);
        juce::DynamicObject::Ptr section = new juce::DynamicObject();
        
        for (int j = 0; j < child.getNumProperties(); ++j) {
            auto name = child.getPropertyName(j);
            section->setProperty(name, child.getProperty(name));
        }
        
        root->setProperty(child.getType(), juce::var(section.get()));
    }
    
    juce::String jsonString = juce::JSON::toString(juce::var(root.get()), true);
    
    if (configFile.replaceWithText(jsonString)) {
        juce::Logger::writeToLog("Config saved to: " + configFile.getFullPathName());
        return true;
    }
    
    juce::Logger::writeToLog("Failed to save config");
    return false;
}

juce::var ConfigManager::getProperty(const juce::Identifier& name, const juce::var& defaultValue) const {
    return config.getProperty(name, defaultValue);
}

void ConfigManager::setProperty(const juce::Identifier& name, const juce::var& value) {
    config.setProperty(name, value, nullptr);
}

} // namespace FXBoard
