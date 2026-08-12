#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <vulkan/vulkan_core.h>

struct Window {
    SDL_Window* handle = nullptr;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    int w = 0;
    int h = 0;
    bool should_close = false;

    Window(const char* title, int w, int h);
    void destroy();

    operator SDL_Window*() const;

    void poll_events();
};