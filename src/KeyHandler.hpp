#pragma once

#include <SDL2/SDL.h>

class KeyHandler {
    bool keys[SDL_NUM_SCANCODES] = {};
public:
    bool operator[](size_t idx) {
        if (idx >= SDL_NUM_SCANCODES) return false;
        return keys[idx];
    };

    void onKeyDown(SDL_Scancode scan_code) {
        if (scan_code >= SDL_NUM_SCANCODES) return;
        keys[scan_code] = true;
    }

    void onKeyUp(SDL_Scancode scan_code) {
        if (scan_code >= SDL_NUM_SCANCODES) return;
        keys[scan_code] = false;
    }
};