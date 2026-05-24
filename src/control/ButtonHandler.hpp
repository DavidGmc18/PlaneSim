#pragma once

#include <stddef.h>

template<size_t size, typename T>
class ButtonHandler {
    bool buttons[size] = {};
    
public:
    bool operator[](size_t button) const {
        if (button >= size) return false;
        return this->buttons[button];
    }

    void onButtonDown(T button) {
        size_t idx = static_cast<size_t>(button);
        if (idx >= size) return;
        this->buttons[idx] = true;
    }

    void onButtonUp(T button) {
        size_t idx = static_cast<size_t>(button);
        if (idx >= size) return;
        this->buttons[idx] = false;
    }
};