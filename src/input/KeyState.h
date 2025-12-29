#pragma once
#include <cstdint>
#include <chrono>

namespace FXBoard {

/**
 * 키 상태 추적
 */
struct KeyState {
    bool pressed = false;
    uint64_t downTs = 0;
    uint64_t upTs = 0;
    
    /**
     * 현재 홀드 시간 (밀리초)
     */
    float getHoldTimeMs() const {
        if (!pressed) return 0.0f;
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::nanoseconds(now.time_since_epoch().count() - downTs);
        return std::chrono::duration<float, std::milli>(duration).count();
    }
    
    void onDown(uint64_t ts) {
        pressed = true;
        downTs = ts;
    }
    
    void onUp(uint64_t ts) {
        pressed = false;
        upTs = ts;
    }
};

} // namespace FXBoard
