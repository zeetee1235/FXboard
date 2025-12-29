#pragma once
#include <juce_audio_basics/juce_audio_basics.h>

namespace FXBoard {

/**
 * 믹서 채널
 */
class MixerChannel {
public:
    MixerChannel() : gain(1.0f), pan(0.0f), mute(false), solo(false) {}
    
    void setGain(float g) { gain = juce::jlimit(0.0f, 2.0f, g); }
    void setPan(float p) { pan = juce::jlimit(-1.0f, 1.0f, p); }
    void setMute(bool m) { mute = m; }
    void setSolo(bool s) { solo = s; }
    
    float getGain() const { return gain; }
    float getPan() const { return pan; }
    bool isMuted() const { return mute; }
    bool isSoloed() const { return solo; }
    
    void process(juce::AudioBuffer<float>& buffer) {
        if (mute) {
            buffer.clear();
            return;
        }
        
        buffer.applyGain(gain);
        
        // 팬 적용 (constant power panning)
        if (buffer.getNumChannels() >= 2 && std::abs(pan) > 0.01f) {
            float angle = (pan + 1.0f) * juce::MathConstants<float>::pi / 4.0f;
            float leftGain = std::cos(angle);
            float rightGain = std::sin(angle);
            
            buffer.applyGain(0, 0, buffer.getNumSamples(), leftGain);
            buffer.applyGain(1, 0, buffer.getNumSamples(), rightGain);
        }
    }
    
private:
    float gain;
    float pan;
    bool mute;
    bool solo;
};

/**
 * 간단한 리미터
 */
class Limiter {
public:
    Limiter() : threshold(-1.0f), release(0.99f) {}
    
    void setThreshold(float thresholdDb) {
        threshold = juce::Decibels::decibelsToGain(thresholdDb);
    }
    
    void setRelease(float releaseCoeff) {
        release = juce::jlimit(0.9f, 0.999f, releaseCoeff);
    }
    
    void process(juce::AudioBuffer<float>& buffer) {
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
            auto* data = buffer.getWritePointer(ch);
            
            for (int i = 0; i < buffer.getNumSamples(); ++i) {
                float input = data[i];
                float absInput = std::abs(input);
                
                // 엔벨로프 추종
                if (absInput > envelope) {
                    envelope = absInput;
                } else {
                    envelope = envelope * release + absInput * (1.0f - release);
                }
                
                // 게인 감소 계산
                float gain = 1.0f;
                if (envelope > threshold) {
                    gain = threshold / envelope;
                }
                
                data[i] = input * gain;
            }
        }
    }
    
    void reset() {
        envelope = 0.0f;
    }
    
private:
    float threshold;
    float release;
    float envelope = 0.0f;
};

/**
 * 믹서
 */
class Mixer {
public:
    Mixer(int numChannels = 8) {
        channels.resize(numChannels);
    }
    
    MixerChannel& getChannel(int index) {
        jassert(index >= 0 && index < static_cast<int>(channels.size()));
        return channels[index];
    }
    
    void setMasterGain(float gain) {
        masterGain = juce::jlimit(0.0f, 2.0f, gain);
    }
    
    float getMasterGain() const { return masterGain; }
    
    Limiter& getMasterLimiter() { return masterLimiter; }
    
    void processMaster(juce::AudioBuffer<float>& buffer) {
        buffer.applyGain(masterGain);
        masterLimiter.process(buffer);
    }
    
private:
    std::vector<MixerChannel> channels;
    float masterGain = 1.0f;
    Limiter masterLimiter;
};

} // namespace FXBoard
