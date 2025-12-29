#pragma once
#include "../core/EventQueue.h"
#include "../input/KeyState.h"
#include "SampleManager.h"
#include "Mixer.h"
#include "FX.h"
#include <juce_audio_devices/juce_audio_devices.h>
#include <array>

namespace FXBoard {

/**
 * 오디오 엔진
 * 저지연 오디오 처리의 핵심
 */
class AudioEngine : public juce::AudioIODeviceCallback {
public:
    static constexpr int MAX_KEYS = 256;
    
    AudioEngine();
    ~AudioEngine() override;
    
    /**
     * 오디오 디바이스 초기화
     */
    bool initialize(int bufferSize = 128);
    
    /**
     * 오디오 시작/중지
     */
    void start();
    void stop();
    
    /**
     * 이벤트 큐 반환 (키 입력용)
     */
    EventQueue& getEventQueue() { return eventQueue; }
    
    /**
     * 이벤트 푸시 (편의 함수)
     */
    void pushEvent(const KeyEvent& event) {
        eventQueue.push(event);
    }
    
    /**
     * 샘플 매니저 반환
     */
    SampleManager& getSampleManager() { return sampleManager; }
    
    /**
     * 키에 샘플 매핑
     */
    void mapKeyToSample(uint32_t scancode, const juce::String& sampleId);
    
    /**
     * FX 활성화/비활성화
     */
    void enableFilter(bool enable) { filterEnabled = enable; }
    void enableBitCrusher(bool enable) { bitCrusherEnabled = enable; }
    void enableReverb(bool enable) { reverbEnabled = enable; }
    
    /**
     * FX 파라미터 설정
     */
    BiquadFilter& getFilter() { return filter; }
    BitCrusher& getBitCrusher() { return bitCrusher; }
    SimpleReverb& getReverb() { return reverb; }
    
    /**
     * 통계 정보
     */
    int getXRunCount() const { return xrunCount; }
    double getCpuLoad() const { return cpuLoad; }
    
    /**
     * 레이턴시 계산
     */
    double getLatencyMs() const {
        auto* device = deviceManager.getCurrentAudioDevice();
        if (device == nullptr) return 0.0;
        
        int bufferSize = device->getCurrentBufferSizeSamples();
        double sampleRate = device->getCurrentSampleRate();
        
        if (sampleRate <= 0.0) return 0.0;
        
        return (bufferSize / sampleRate) * 1000.0;
    }
    
    // AudioIODeviceCallback 오버라이드
    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                         int numInputChannels,
                                         float* const* outputChannelData,
                                         int numOutputChannels,
                                         int numSamples,
                                         const juce::AudioIODeviceCallbackContext& context) override;
    
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;
    
private:
    juce::AudioDeviceManager deviceManager;
    EventQueue eventQueue;
    SampleManager sampleManager;
    SamplePlayer samplePlayer;
    Mixer mixer;
    
    // FX
    BiquadFilter filter;
    BitCrusher bitCrusher;
    SimpleReverb reverb;
    
    bool filterEnabled = false;
    bool bitCrusherEnabled = false;
    bool reverbEnabled = false;
    
    // 키 상태
    std::array<KeyState, MAX_KEYS> keyStates;
    std::array<juce::String, MAX_KEYS> keyToSampleMap;
    
    // 통계
    std::atomic<int> xrunCount{0};
    std::atomic<double> cpuLoad{0.0};
    
    void processEvents();
    void processAudio(float* const* outputChannelData, int numOutputChannels, int numSamples);
};

} // namespace FXBoard
