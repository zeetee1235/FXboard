#include "KeyHook.h"
#include <chrono>

#if JUCE_LINUX
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#endif

namespace FXBoard {

KeyHook::KeyHook() : active(false) {
#if JUCE_WINDOWS
    hookHandle = nullptr;
#elif JUCE_LINUX
    deviceFd = -1;
#elif JUCE_MAC
    eventTap = nullptr;
#endif
}

KeyHook::~KeyHook() {
    stop();
}

uint64_t KeyHook::getCurrentTimestampNs() {
    auto now = std::chrono::high_resolution_clock::now();
    return static_cast<uint64_t>(now.time_since_epoch().count());
}

void KeyHook::processKeyDown(uint32_t scancode) {
    KeyEvent event(KeyEvent::Down, scancode, getCurrentTimestampNs());
    eventQueue.push(event);
}

void KeyHook::processKeyUp(uint32_t scancode) {
    KeyEvent event(KeyEvent::Up, scancode, getCurrentTimestampNs());
    eventQueue.push(event);
}

#if JUCE_LINUX

bool KeyHook::start() {
    if (active) return true;
    
    // /dev/input/event* 디바이스 열기
    // 실제 구현에서는 적절한 이벤트 디바이스를 찾아야 함
    const char* device = "/dev/input/event0"; // 예시
    deviceFd = open(device, O_RDONLY | O_NONBLOCK);
    
    if (deviceFd < 0) {
        juce::Logger::writeToLog("Failed to open input device: " + juce::String(device));
        return false;
    }
    
    // 입력 읽기 스레드는 나중에 구현
    // TODO: 입력 읽기 스레드 시작 로직 구현
    
    active = true;
    return true;
}

void KeyHook::stop() {
    if (!active) return;
    
    if (deviceFd >= 0) {
        close(deviceFd);
        deviceFd = -1;
    }
    
    active = false;
}

void KeyHook::readInputEvents() {
    struct input_event ev;
    while (active) {
        ssize_t n = read(deviceFd, &ev, sizeof(ev));
        if (n == sizeof(ev)) {
            if (ev.type == EV_KEY) {
                if (ev.value == 1) { // Key down
                    processKeyDown(ev.code);
                } else if (ev.value == 0) { // Key up
                    processKeyUp(ev.code);
                }
                // ev.value == 2는 리피트, 무시
            }
        }
        juce::Thread::sleep(1); // CPU 사용률 낮추기
    }
}

#elif JUCE_WINDOWS

// Windows 구현
bool KeyHook::start() {
    if (active) return true;
    
    // Low-level 키보드 후크 설치
    // TODO: Windows API 구현
    
    active = true;
    return true;
}

void KeyHook::stop() {
    if (!active) return;
    
    // 후크 해제
    // TODO: Windows API 구현
    
    active = false;
}

#elif JUCE_MAC

// macOS 구현
bool KeyHook::start() {
    if (active) return true;
    
    // CGEvent tap 생성
    // TODO: macOS API 구현
    
    active = true;
    return true;
}

void KeyHook::stop() {
    if (!active) return;
    
    // Event tap 해제
    // TODO: macOS API 구현
    
    active = false;
}

#else

// 기본 구현 (비활성)
bool KeyHook::start() {
    juce::Logger::writeToLog("KeyHook not implemented for this platform");
    return false;
}

void KeyHook::stop() {
}

#endif

} // namespace FXBoard
