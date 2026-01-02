#pragma once
#include <juce_audio_formats/juce_audio_formats.h>
#include <map>
#include <memory>

namespace FXBoard {

/**
 * 샘플 데이터 구조
 */
struct Sample {
    juce::String id;
    juce::AudioBuffer<float> buffer;
    double sampleRate = 48000.0;
    
    bool isValid() const {
        return buffer.getNumSamples() > 0;
    }
};

/**
 * 샘플 관리자
 * WAV/FLAC 파일을 로드하고 메모리에 상주
 */
class SampleManager {
public:
    SampleManager();
    ~SampleManager();
    
    /**
     * 샘플 로드
     * @param id 샘플 ID (참조용)
     * @param filePath 오디오 파일 경로
     * @return 성공 시 true
     */
    bool loadSample(const juce::String& id, const juce::File& filePath);
    
    /**
     * 샘플 가져오기
     */
    const Sample* getSample(const juce::String& id) const;
    
    /**
     * 모든 샘플 제거
     */
    void clear();
    
    /**
     * 샘플 개수
     */
    int getNumSamples() const { return static_cast<int>(samples.size()); }
    
    /**
     * 모든 샘플 ID 반환
     */
    juce::StringArray getAllSampleIds() const;
    
    /**
     * 샘플 게인 설정/가져오기
     */
    void setSampleGain(const juce::String& id, float gain);
    float getSampleGain(const juce::String& id) const;
    
private:
    juce::AudioFormatManager formatManager;
    std::map<juce::String, std::unique_ptr<Sample>> samples;
    std::map<juce::String, float> sampleGains;
};

/**
 * 샘플 보이스 (폴리포닉 재생)
 */
class SampleVoice {
public:
    SampleVoice() : isPlaying(false), position(0.0), gain(1.0f) {}
    
    void trigger(const Sample* sample, float velocity = 1.0f);
    void stop();
    bool isActive() const { return isPlaying; }
    
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, 
                         int startSample, int numSamples);
    
private:
    const Sample* currentSample = nullptr;
    bool isPlaying;
    double position;
    float gain;
};

/**
 * 샘플 플레이어
 * 여러 샘플을 동시에 재생 (폴리포니)
 */
class SamplePlayer {
public:
    SamplePlayer(int maxVoices = 16);
    
    void trigger(const Sample* sample, float velocity = 1.0f);
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, 
                         int startSample, int numSamples);
    
private:
    std::vector<std::unique_ptr<SampleVoice>> voices;
    int findFreeVoice();
};

} // namespace FXBoard
