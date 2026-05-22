#pragma once

#include <SDL2/SDL.h>

class KeyHandler {
protected:
    bool keys[SDL_NUM_SCANCODES] = {};
public:
    bool getKey(unsigned idx) const;
    virtual void onKeyDown(SDL_Scancode code);
    virtual void onKeyUp(SDL_Scancode code);
};