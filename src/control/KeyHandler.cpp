#include "KeyHandler.hpp"

bool KeyHandler::getKey(unsigned idx) const {
    if (idx >= SDL_NUM_SCANCODES) return false;
    return this->keys[idx];
};

void KeyHandler::onKeyDown(SDL_Scancode code) {
    if (code >= SDL_NUM_SCANCODES) return;
    this->keys[code] = true;
}

void KeyHandler::onKeyUp(SDL_Scancode code) {
    if (code >= SDL_NUM_SCANCODES) return;
    this->keys[code] = false;
}