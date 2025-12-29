#pragma once
#include <cstdint>

namespace FXBoard {

struct KeyEvent {
    enum Type {
        Down,
        Up
    } type;
    
    uint32_t scancode;
    uint64_t timestampNs;
    
    KeyEvent() : type(Down), scancode(0), timestampNs(0) {}
    KeyEvent(Type t, uint32_t sc, uint64_t ts) 
        : type(t), scancode(sc), timestampNs(ts) {}
};

} // namespace FXBoard
