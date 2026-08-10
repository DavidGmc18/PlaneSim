#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <vulkan/vulkan_core.h>

struct Window {
    SDL_Window* handle = nullptr;
    int w = 0;
    int h = 0;

    Window(const char* title, int w, int h);
    ~Window();
};