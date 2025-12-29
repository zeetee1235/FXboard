#pragma once
#include "../core/Smoother.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>

namespace FXBoard {

/**
 * 1-pole Low Pass Filter
 * 간단하고 효율적인 저역 필터
 */
class OnePoleLPF {
public:
    void setup(double sr) { 
        sampleRate = sr; 
        cutoffSmoother.setTimeMs(5.0f, static_cast<float>(sr));
    }
    
    void setCutoff(float hz) { 
        cutoffSmoother.setTarget(juce::jlimit(20.0f, 20000.0f, hz));
    }
    
    float process(float x) {
        // 스무드한 컷오프 변화
        float cutoff = cutoffSmoother.step();
        
        // 1-pole 계수 계산
        float a = cutoff / (cutoff + static_cast<float>(sampleRate));
        y += a * (x - y);
        return y;
    }
    
    void reset() {
        y = 0.0f;
        cutoffSmoother.setValue(300.0f);
    }
    
private:
    double sampleRate = 48000.0;
    Smoother cutoffSmoother;
    float y = 0.0f;
};

/**
 * Biquad Filter (2-pole)
 * 더 강력한 필터링, 공진(resonance) 지원
 */
class BiquadFilter {
public:
    enum Type {
        LowPass,
        HighPass,
        BandPass
    };
    
    void setup(double sr, Type t = LowPass) {
        sampleRate = sr;
        filterType = t;
        cutoffSmoother.setTimeMs(5.0f, static_cast<float>(sr));
        resSmoother.setTimeMs(5.0f, static_cast<float>(sr));
        updateCoefficients();
    }
    
    void setCutoff(float hz) {
        cutoffSmoother.setTarget(juce::jlimit(20.0f, 20000.0f, hz));
    }
    
    void setResonance(float q) {
        resSmoother.setTarget(juce::jlimit(0.1f, 10.0f, q));
    }
    
    float process(float x) {
        // 파라미터 스무딩
        float cutoff = cutoffSmoother.step();
        float q = resSmoother.step();
        
        // 계수가 변경되었으면 업데이트
        if (std::abs(cutoff - lastCutoff) > 1.0f || 
            std::abs(q - lastQ) > 0.01f) {
            lastCutoff = cutoff;
            lastQ = q;
            updateCoefficients();
        }
        
        // Biquad 필터 처리
        float out = b0 * x + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
        
        // 상태 업데이트
        x2 = x1;
        x1 = x;
        y2 = y1;
        y1 = out;
        
        return out;
    }
    
    void reset() {
        x1 = x2 = y1 = y2 = 0.0f;
    }
    
private:
    void updateCoefficients() {
        float omega = 2.0f * juce::MathConstants<float>::pi * lastCutoff / static_cast<float>(sampleRate);
        float sn = std::sin(omega);
        float cs = std::cos(omega);
        float alpha = sn / (2.0f * lastQ);
        
        if (filterType == LowPass) {
            b0 = (1.0f - cs) / 2.0f;
            b1 = 1.0f - cs;
            b2 = (1.0f - cs) / 2.0f;
        } else if (filterType == HighPass) {
            b0 = (1.0f + cs) / 2.0f;
            b1 = -(1.0f + cs);
            b2 = (1.0f + cs) / 2.0f;
        } else { // BandPass
            b0 = alpha;
            b1 = 0.0f;
            b2 = -alpha;
        }
        
        float a0 = 1.0f + alpha;
        a1 = -2.0f * cs / a0;
        a2 = (1.0f - alpha) / a0;
        
        b0 /= a0;
        b1 /= a0;
        b2 /= a0;
    }
    
    double sampleRate = 48000.0;
    Type filterType = LowPass;
    
    Smoother cutoffSmoother;
    Smoother resSmoother;
    
    float lastCutoff = 1000.0f;
    float lastQ = 0.707f;
    
    // Biquad 계수
    float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
    float a1 = 0.0f, a2 = 0.0f;
    
    // 상태 변수
    float x1 = 0.0f, x2 = 0.0f;
    float y1 = 0.0f, y2 = 0.0f;
};

/**
 * Bit Crusher
 * 비트 뎁스 및 샘플레이트 감소 효과
 */
class BitCrusher {
public:
    void setup(double sr) {
        sampleRate = sr;
        bitDepthSmoother.setTimeMs(5.0f, static_cast<float>(sr));
        downsampleSmoother.setTimeMs(5.0f, static_cast<float>(sr));
    }
    
    void setBitDepth(float bits) {
        bitDepthSmoother.setTarget(juce::jlimit(1.0f, 16.0f, bits));
    }
    
    void setDownsample(float factor) {
        downsampleSmoother.setTarget(juce::jlimit(1.0f, 16.0f, factor));
    }
    
    float process(float x) {
        float bitDepth = bitDepthSmoother.step();
        float downsample = downsampleSmoother.step();
        
        // Downsampling
        downsampleCounter += 1.0f;
        if (downsampleCounter >= downsample) {
            downsampleCounter -= downsample;
            
            // Bit depth reduction
            float levels = std::pow(2.0f, bitDepth);
            lastSample = std::floor(x * levels) / levels;
        }
        
        return lastSample;
    }
    
    void reset() {
        lastSample = 0.0f;
        downsampleCounter = 0.0f;
    }
    
private:
    double sampleRate = 48000.0;
    Smoother bitDepthSmoother;
    Smoother downsampleSmoother;
    
    float lastSample = 0.0f;
    float downsampleCounter = 0.0f;
};

/**
 * Simple Reverb
 * 간단한 Freeverb 스타일 리버브
 */
class SimpleReverb {
public:
    void setup(double sr) {
        sampleRate = sr;
        mixSmoother.setTimeMs(50.0f, static_cast<float>(sr)); // 느린 페이드
        
        // 간단한 딜레이 라인 (실제로는 더 복잡한 구현 필요)
        delayBufferSize = static_cast<int>(sr * 0.05); // 50ms
        delayBuffer.resize(delayBufferSize, 0.0f);
    }
    
    void setMix(float m) {
        mixSmoother.setTarget(juce::jlimit(0.0f, 1.0f, m));
    }
    
    void setDecay(float d) {
        decay = juce::jlimit(0.0f, 0.99f, d);
    }
    
    float process(float x) {
        float mix = mixSmoother.step();
        
        // 간단한 컴 필터
        int readPos = (writePos + 1) % delayBufferSize;
        float delayed = delayBuffer[readPos];
        float output = x + delayed * decay;
        
        delayBuffer[writePos] = output;
        writePos = (writePos + 1) % delayBufferSize;
        
        // Dry/Wet 믹싱
        return x * (1.0f - mix) + output * mix;
    }
    
    void reset() {
        std::fill(delayBuffer.begin(), delayBuffer.end(), 0.0f);
        writePos = 0;
    }
    
private:
    double sampleRate = 48000.0;
    Smoother mixSmoother;
    float decay = 0.5f;
    
    std::vector<float> delayBuffer;
    int delayBufferSize = 0;
    int writePos = 0;
};

} // namespace FXBoard
