#include "KeyHook.h"
#include <chrono>
#include <thread>

#if JUCE_LINUX
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <dirent.h>
#include <cstring>
#include <X11/Xlib.h>
#include <X11/keysym.h>
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

// /dev/input에서 키보드 디바이스 찾기 (개선된 버전)
int KeyHook::findKeyboardDevice() {
    DIR* dir = opendir("/dev/input");
    if (!dir) {
        juce::Logger::writeToLog("Failed to open /dev/input directory");
        juce::Logger::writeToLog("You may need to run: sudo ./scripts/setup_permissions.sh");
        return -1;
    }
    
    struct dirent* entry;
    int fd = -1;
    int bestScore = 0;
    
    // Try to find the best keyboard device (prefer actual keyboards over mice, etc.)
    while ((entry = readdir(dir)) != nullptr) {
        if (strncmp(entry->d_name, "event", 5) != 0) {
            continue;
        }
        
        char path[256];
        snprintf(path, sizeof(path), "/dev/input/%s", entry->d_name);
        
        int testFd = open(path, O_RDONLY | O_NONBLOCK);
        if (testFd < 0) {
            // Log permission errors for debugging
            if (errno == EACCES || errno == EPERM) {
                juce::Logger::writeToLog(juce::String("Permission denied for: ") + juce::String(path));
            }
            continue;
        }
        
        // Get device name
        char name[256] = "Unknown";
        if (ioctl(testFd, EVIOCGNAME(sizeof(name)), name) < 0) {
            close(testFd);
            continue;
        }
        
        juce::Logger::writeToLog(juce::String("Checking input device: ") + juce::String(path) + 
                                 " - " + juce::String(name));
        
        // Check if device supports keyboard events
        unsigned long evbit[EV_MAX/sizeof(long)/8 + 1] = {0};
        if (ioctl(testFd, EVIOCGBIT(0, sizeof(evbit)), evbit) < 0) {
            close(testFd);
            continue;
        }
        
        if (!(evbit[0] & (1 << EV_KEY))) {
            close(testFd);
            continue;
        }
        
        // Check for actual keyboard keys (not just mouse buttons)
        unsigned long keybit[KEY_MAX/sizeof(long)/8 + 1] = {0};
        if (ioctl(testFd, EVIOCGBIT(EV_KEY, sizeof(keybit)), keybit) < 0) {
            close(testFd);
            continue;
        }
        
        // Score the device based on how keyboard-like it is
        int score = 0;
        
        // Check for common keyboard keys (using proper bit testing)
        auto test_bit = [&](unsigned int bit) -> bool {
            return !!(keybit[bit / (sizeof(long) * 8)] & (1UL << (bit % (sizeof(long) * 8))));
        };
        
        if (test_bit(KEY_A)) score += 10;
        if (test_bit(KEY_SPACE)) score += 10;
        if (test_bit(KEY_ENTER)) score += 10;
        
        // Penalize devices with mouse-like names
        juce::String deviceName(name);
        if (deviceName.containsIgnoreCase("mouse") || deviceName.containsIgnoreCase("touchpad")) {
            score -= 50;
        }
        // Prefer devices with keyboard-like names
        if (deviceName.containsIgnoreCase("keyboard") || deviceName.containsIgnoreCase("kbd")) {
            score += 20;
        }
        
        juce::Logger::writeToLog(juce::String("  Device score: ") + juce::String(score));
        
        if (score > bestScore) {
            if (fd >= 0) {
                close(fd);  // Close previous best
            }
            fd = testFd;
            bestScore = score;
            juce::Logger::writeToLog(juce::String("  ^ New best keyboard device"));
        } else {
            close(testFd);
        }
    }
    
    closedir(dir);
    
    if (fd >= 0) {
        juce::Logger::writeToLog("Successfully opened keyboard device");
    } else {
        juce::Logger::writeToLog("Failed to find suitable keyboard device");
        juce::Logger::writeToLog("Make sure:");
        juce::Logger::writeToLog("  1. Your user is in the 'input' group: groups | grep input");
        juce::Logger::writeToLog("  2. udev rules are installed: ls /etc/udev/rules.d/99-fxboard.rules");
        juce::Logger::writeToLog("  3. You've logged out and back in after adding to group");
        juce::Logger::writeToLog("Run: sudo ./scripts/setup_permissions.sh");
    }
    
    return fd;
}

void KeyHook::runHookThread() {
    struct input_event ev;
    
    while (active) {
        ssize_t n = read(deviceFd, &ev, sizeof(ev));
        
        if (n == sizeof(ev)) {
            if (ev.type == EV_KEY) {
                // ev.code는 이미 Linux 스캔코드 (evdev codes)
                // 우리가 사용하는 스캔코드와 동일
                if (ev.value == 1) {  // Key press
                    processKeyDown(ev.code);
                    juce::Logger::writeToLog(juce::String("Key down: ") + juce::String((int)ev.code));
                } else if (ev.value == 0) {  // Key release
                    processKeyUp(ev.code);
                }
                // ev.value == 2는 key repeat (무시)
            }
        } else if (n < 0 && errno != EAGAIN) {
            juce::Logger::writeToLog("Error reading from input device");
            break;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

bool KeyHook::start() {
    if (active) return true;
    
    // 키보드 디바이스 찾기
    deviceFd = findKeyboardDevice();
    
    if (deviceFd < 0) {
        juce::Logger::writeToLog("Failed to find keyboard device. Try running with sudo or add user to 'input' group:");
        juce::Logger::writeToLog("  sudo usermod -a -G input $USER");
        juce::Logger::writeToLog("  (then logout and login again)");
        return false;
    }
    
    // 후킹 스레드 시작
    active = true;
    hookThread = std::make_unique<std::thread>(&KeyHook::runHookThread, this);
    
    juce::Logger::writeToLog("Global keyboard hook started (evdev)");
    return true;
}

void KeyHook::stop() {
    if (!active) return;
    
    active = false;
    
    if (hookThread && hookThread->joinable()) {
        hookThread->join();
    }
    
    if (deviceFd >= 0) {
        close(deviceFd);
        deviceFd = -1;
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
