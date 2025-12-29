#pragma once
#include <cmath>

namespace FXBoard {

/**
 * 파라미터 스무딩 유틸리티
 * 급격한 파라미터 변화로 인한 클릭/노이즈 방지
 */
struct Smoother {
    float value = 0.0f;
    float target = 0.0f;
    float coeff = 0.0f; // 0..1 per-sample
    
    /**
     * 스무딩 시간 설정
     * @param ms 밀리초 단위 스무딩 시간 (권장: 3-10ms)
     * @param sampleRate 샘플레이트 (Hz)
     */
    void setTimeMs(float ms, float sampleRate) {
        float samples = ms * sampleRate / 1000.0f;
        coeff = (samples > 1.0f) ? 1.0f / samples : 1.0f;
    }
    
    /**
     * 타겟 값 설정
     */
    void setTarget(float t) {
        target = t;
    }
    
    /**
     * 현재 값 즉시 설정 (스무딩 없이)
     */
    void setValue(float v) {
        value = v;
        target = v;
    }
    
    /**
     * 다음 샘플 값 계산
     */
    inline float step() {
        value += coeff * (target - value);
        return value;
    }
    
    /**
     * 타겟에 도달했는지 확인
     */
    bool isAtTarget(float epsilon = 0.001f) const {
        return std::abs(value - target) < epsilon;
    }
};

} // namespace FXBoard
