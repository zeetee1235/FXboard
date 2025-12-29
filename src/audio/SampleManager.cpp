#include "SampleManager.h"

namespace FXBoard {

SampleManager::SampleManager() {
    formatManager.registerBasicFormats();
}

SampleManager::~SampleManager() {
    clear();
}

bool SampleManager::loadSample(const juce::String& id, const juce::File& filePath) {
    if (!filePath.existsAsFile()) {
        juce::Logger::writeToLog("Sample file not found: " + filePath.getFullPathName());
        return false;
    }
    
    std::unique_ptr<juce::AudioFormatReader> reader(
        formatManager.createReaderFor(filePath));
    
    if (reader == nullptr) {
        juce::Logger::writeToLog("Failed to create reader for: " + filePath.getFullPathName());
        return false;
    }
    
    auto sample = std::make_unique<Sample>();
    sample->id = id;
    sample->sampleRate = reader->sampleRate;
    sample->buffer.setSize(static_cast<int>(reader->numChannels), 
                          static_cast<int>(reader->lengthInSamples));
    
    reader->read(&sample->buffer, 0, static_cast<int>(reader->lengthInSamples), 
                 0, true, true);
    
    // 시작 부분에 짧은 페이드-인 (클릭 방지)
    int fadeLength = juce::jmin(128, sample->buffer.getNumSamples());
    for (int ch = 0; ch < sample->buffer.getNumChannels(); ++ch) {
        auto* data = sample->buffer.getWritePointer(ch);
        for (int i = 0; i < fadeLength; ++i) {
            float gain = static_cast<float>(i) / static_cast<float>(fadeLength);
            data[i] *= gain;
        }
    }
    
    samples[id] = std::move(sample);
    juce::Logger::writeToLog("Loaded sample: " + id + " from " + filePath.getFileName());
    return true;
}

const Sample* SampleManager::getSample(const juce::String& id) const {
    auto it = samples.find(id);
    if (it != samples.end()) {
        return it->second.get();
    }
    return nullptr;
}

void SampleManager::clear() {
    samples.clear();
}

// SampleVoice 구현

void SampleVoice::trigger(const Sample* sample, float velocity) {
    if (sample == nullptr || !sample->isValid()) return;
    
    currentSample = sample;
    position = 0.0;
    gain = velocity;
    isPlaying = true;
}

void SampleVoice::stop() {
    isPlaying = false;
    currentSample = nullptr;
}

void SampleVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, 
                                   int startSample, int numSamples) {
    if (!isPlaying || currentSample == nullptr) return;
    
    const auto& sourceBuffer = currentSample->buffer;
    int sourceChannels = sourceBuffer.getNumChannels();
    int outputChannels = outputBuffer.getNumChannels();
    int sourceSamples = sourceBuffer.getNumSamples();
    
    for (int i = 0; i < numSamples; ++i) {
        int pos = static_cast<int>(position);
        if (pos >= sourceSamples) {
            isPlaying = false;
            break;
        }
        
        // 간단한 선형 보간
        float frac = static_cast<float>(position - pos);
        int nextPos = juce::jmin(pos + 1, sourceSamples - 1);
        
        for (int ch = 0; ch < outputChannels; ++ch) {
            int srcCh = juce::jmin(ch, sourceChannels - 1);
            const float* srcData = sourceBuffer.getReadPointer(srcCh);
            float* outData = outputBuffer.getWritePointer(ch);
            
            float sample = srcData[pos] * (1.0f - frac) + srcData[nextPos] * frac;
            outData[startSample + i] += sample * gain;
        }
        
        position += 1.0; // 샘플레이트 변환은 나중에 추가 가능
    }
}

// SamplePlayer 구현

SamplePlayer::SamplePlayer(int maxVoices) {
    voices.reserve(maxVoices);
    for (int i = 0; i < maxVoices; ++i) {
        voices.push_back(std::make_unique<SampleVoice>());
    }
}

void SamplePlayer::trigger(const Sample* sample, float velocity) {
    int voiceIndex = findFreeVoice();
    if (voiceIndex >= 0) {
        voices[voiceIndex]->trigger(sample, velocity);
    }
}

void SamplePlayer::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, 
                                    int startSample, int numSamples) {
    for (auto& voice : voices) {
        if (voice->isActive()) {
            voice->renderNextBlock(outputBuffer, startSample, numSamples);
        }
    }
}

int SamplePlayer::findFreeVoice() {
    for (size_t i = 0; i < voices.size(); ++i) {
        if (!voices[i]->isActive()) {
            return static_cast<int>(i);
        }
    }
    // 가장 오래된 보이스 교체 (간단한 보이스 스틸링)
    return 0;
}

} // namespace FXBoard
