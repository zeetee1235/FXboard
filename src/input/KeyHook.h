#pragma once
#include "../core/EventQueue.h"
#include <juce_events/juce_events.h>
#include <memory>

namespace FXBoard {

/**
 * 키보드 입력 후킹
 * 플랫폼별 저수준 키보드 입력 수집
 */
class KeyHook {
public:
    KeyHook();
    ~KeyHook();
    
    /**
     * 키 후킹 시작
     */
    bool start();
    
    /**
     * 키 후킹 중지
     */
    void stop();
    
    /**
     * 이벤트 큐 반환
     */
    EventQueue& getEventQueue() { return eventQueue; }
    
    /**
     * 후킹 활성 상태 확인
     */
    bool isActive() const { return active; }
    
private:
    EventQueue eventQueue;
    bool active;
    
    // 플랫폼별 구현
#if JUCE_WINDOWS
    void* hookHandle;
    static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
#elif JUCE_LINUX
    int deviceFd;  // evdev file descriptor
    std::unique_ptr<std::thread> hookThread;
    void runHookThread();
    int findKeyboardDevice();
#elif JUCE_MAC
    void* eventTap;
    static CGEventRef eventCallback(CGEventTapProxy proxy, CGEventType type, 
                                    CGEventRef event, void* refcon);
#endif
    
    void processKeyDown(uint32_t scancode);
    void processKeyUp(uint32_t scancode);
    uint64_t getCurrentTimestampNs();
};

} // namespace FXBoard
