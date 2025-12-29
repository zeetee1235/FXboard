#pragma once
#include "KeyEvent.h"
#include <array>
#include <atomic>
#include <cstddef>

namespace FXBoard {

/**
 * 락프리 Multi-Producer Single-Consumer 이벤트 큐
 * 키보드 입력 스레드에서 오디오 스레드로 이벤트 전달
 */
class EventQueue {
public:
    EventQueue() : tail(0), head(0) {}
    
    /**
     * 이벤트를 큐에 추가 (프로듀서)
     * @return 성공 시 true, 큐가 가득 차면 false
     */
    bool push(const KeyEvent& e) {
        auto next = (head + 1) & mask;
        if (next == tail.load(std::memory_order_acquire)) {
            return false; // 큐 가득 참
        }
        buffer[head] = e;
        head = next;
        return true;
    }
    
    /**
     * 이벤트를 큐에서 제거 (컨슈머)
     * @return 성공 시 true, 큐가 비어있으면 false
     */
    bool pop(KeyEvent& out) {
        auto t = tail.load(std::memory_order_acquire);
        if (t == head) {
            return false; // 큐 비어있음
        }
        out = buffer[t];
        tail.store((t + 1) & mask, std::memory_order_release);
        return true;
    }
    
    bool isEmpty() const {
        return tail.load(std::memory_order_acquire) == head;
    }
    
private:
    static constexpr size_t capacity = 1024;
    static constexpr size_t mask = capacity - 1;
    
    std::array<KeyEvent, capacity> buffer{};
    std::atomic<size_t> tail;
    size_t head;
};

} // namespace FXBoard
