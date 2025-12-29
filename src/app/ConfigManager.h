#pragma once
#include <juce_data_structures/juce_data_structures.h>

namespace FXBoard {

/**
 * 설정 관리자
 * JSON 기반 설정 로드/저장
 */
class ConfigManager {
public:
    ConfigManager();
    
    /**
     * 설정 파일 로드
     */
    bool loadConfig(const juce::File& configFile);
    
    /**
     * 설정 파일 저장
     */
    bool saveConfig(const juce::File& configFile);
    
    /**
     * 설정 값 가져오기
     */
    juce::var getProperty(const juce::Identifier& name, const juce::var& defaultValue = juce::var()) const;
    
    /**
     * 설정 값 설정
     */
    void setProperty(const juce::Identifier& name, const juce::var& value);
    
    /**
     * 전체 설정 트리 반환
     */
    juce::ValueTree& getValueTree() { return config; }
    const juce::ValueTree& getValueTree() const { return config; }
    
private:
    juce::ValueTree config;
};

} // namespace FXBoard
