#include "AudioEngine.h"

namespace FXBoard {

AudioEngine::AudioEngine() : samplePlayer(16) {
    mixer.getMasterLimiter().setThreshold(-1.0f);
}

AudioEngine::~AudioEngine() {
    stop();
}

bool AudioEngine::initialize(int bufferSize) {
    // 오디오 디바이스 초기화 - 가장 간단한 방법
    juce::String error = deviceManager.initialiseWithDefaultDevices(0, 2);
    
    if (error.isNotEmpty()) {
        juce::Logger::writeToLog("Audio device error: " + error);
        return false;
    }
    
    // 버퍼 크기 설정
    auto* device = deviceManager.getCurrentAudioDevice();
    if (device != nullptr) {
        juce::AudioDeviceManager::AudioDeviceSetup setup;
        deviceManager.getAudioDeviceSetup(setup);
        
        setup.bufferSize = bufferSize;
        setup.sampleRate = 48000.0;
        
        error = deviceManager.setAudioDeviceSetup(setup, true);
        if (error.isNotEmpty()) {
            juce::Logger::writeToLog("Audio setup warning: " + error);
            // 경고만 표시하고 계속 진행
        }
    }
    
    // FX 초기화
    double sampleRate = 48000.0;
    if (auto* device = deviceManager.getCurrentAudioDevice()) {
        sampleRate = device->getCurrentSampleRate();
    }
    
    filter.setup(sampleRate, BiquadFilter::LowPass);
    filter.setCutoff(1000.0f);
    filter.setResonance(0.707f);
    
    bitCrusher.setup(sampleRate);
    bitCrusher.setBitDepth(16.0f);
    bitCrusher.setDownsample(1.0f);
    
    reverb.setup(sampleRate);
    reverb.setMix(0.0f);
    reverb.setDecay(0.5f);
    
    juce::Logger::writeToLog(juce::String("Audio initialized: ") + 
                            juce::String(sampleRate) + " Hz, " + 
                            juce::String(bufferSize) + " samples");
    
    return true;
}

void AudioEngine::start() {
    deviceManager.addAudioCallback(this);
}

void AudioEngine::stop() {
    deviceManager.removeAudioCallback(this);
}

void AudioEngine::mapKeyToSample(uint32_t scancode, const juce::String& sampleId) {
    if (scancode < MAX_KEYS) {
        keyToSampleMap[scancode] = sampleId;
    }
}

void AudioEngine::unmapKey(uint32_t scancode) {
    if (scancode < MAX_KEYS) {
        keyToSampleMap[scancode] = juce::String();
    }
}

std::map<uint32_t, juce::String> AudioEngine::getKeyMappings() const {
    std::map<uint32_t, juce::String> mappings;
    for (uint32_t i = 0; i < MAX_KEYS; ++i) {
        if (keyToSampleMap[i].isNotEmpty()) {
            mappings[i] = keyToSampleMap[i];
        }
    }
    return mappings;
}

void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device) {
    juce::Logger::writeToLog("Audio device started: " + device->getName());
    xrunCount = 0;
}

void AudioEngine::audioDeviceStopped() {
    juce::Logger::writeToLog("Audio device stopped");
}

void AudioEngine::audioDeviceIOCallbackWithContext(
    const float* const* inputChannelData,
    int numInputChannels,
    float* const* outputChannelData,
    int numOutputChannels,
    int numSamples,
    const juce::AudioIODeviceCallbackContext& context) 
{
    juce::ignoreUnused(inputChannelData, numInputChannels, context);
    
    auto startTime = juce::Time::getHighResolutionTicks();
    
    // 이벤트 처리
    processEvents();
    
    // 오디오 처리
    processAudio(outputChannelData, numOutputChannels, numSamples);
    
    // CPU 부하 계산
    auto endTime = juce::Time::getHighResolutionTicks();
    double elapsed = juce::Time::highResolutionTicksToSeconds(endTime - startTime);
    double bufferDuration = numSamples / 48000.0;
    cpuLoad = (elapsed / bufferDuration) * 100.0;
}

void AudioEngine::processEvents() {
    KeyEvent event;
    int eventCount = 0;
    while (eventQueue.pop(event)) {
        eventCount++;
        if (event.scancode >= MAX_KEYS) {
            juce::Logger::writeToLog("Event scancode too large: " + juce::String(event.scancode));
            continue;
        }
        
        auto& keyState = keyStates[event.scancode];
        
        if (event.type == KeyEvent::Down) {
            keyState.onDown(event.timestampNs);
            
            // 샘플 트리거
            const auto& sampleId = keyToSampleMap[event.scancode];
            juce::Logger::writeToLog("Processing key down - scancode: " + juce::String(event.scancode) + 
                                     ", sampleId: " + sampleId);
            if (sampleId.isNotEmpty()) {
                const Sample* sample = sampleManager.getSample(sampleId);
                if (sample != nullptr) {
                    juce::Logger::writeToLog("Triggering sample: " + sampleId + 
                                           ", samples: " + juce::String(sample->buffer.getNumSamples()));
                    samplePlayer.trigger(sample, 1.0f);
                } else {
                    juce::Logger::writeToLog("Sample not found: " + sampleId);
                }
            } else {
                juce::Logger::writeToLog("No sample mapped for scancode: " + juce::String(event.scancode));
            }
            
        } else if (event.type == KeyEvent::Up) {
            keyState.onUp(event.timestampNs);
        }
    }
    if (eventCount > 0) {
        juce::Logger::writeToLog("Processed " + juce::String(eventCount) + " events");
    }
}

void AudioEngine::processAudio(float* const* outputChannelData, 
                                int numOutputChannels, 
                                int numSamples) {
    // 임시 버퍼 생성
    juce::AudioBuffer<float> buffer(outputChannelData, numOutputChannels, numSamples);
    buffer.clear();
    
    // 샘플 렌더링
    samplePlayer.renderNextBlock(buffer, 0, numSamples);
    
    // FX 체인 적용
    if (filterEnabled) {
        for (int ch = 0; ch < numOutputChannels; ++ch) {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i) {
                data[i] = filter.process(data[i]);
            }
        }
    }
    
    if (bitCrusherEnabled) {
        for (int ch = 0; ch < numOutputChannels; ++ch) {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i) {
                data[i] = bitCrusher.process(data[i]);
            }
        }
    }
    
    if (reverbEnabled) {
        for (int ch = 0; ch < numOutputChannels; ++ch) {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i) {
                data[i] = reverb.process(data[i]);
            }
        }
    }
    
    // 마스터 믹서 처리 (리미터 포함)
    mixer.processMaster(buffer);
}

} // namespace FXBoard
