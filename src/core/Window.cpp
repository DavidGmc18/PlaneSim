#include "Window.hpp"
#include <SDL3/SDL_video.h>
#include <vulkan/vulkan_core.h>
#include "common.hpp"

Window::Window(const char* title, int w, int h) {
    this->handle = CHK(SDL_CreateWindow(title, w, h, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN));
    CHK(SDL_GetWindowSizeInPixels(this->handle, &this->w, &this->h));
}

Window::~Window() {
    SDL_DestroyWindow(this->handle);
}