#include "Window.hpp"
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_vulkan.h>
#include "common.hpp"
#include "vk/vk.hpp"

Window::Window(const char* title, int w, int h) {
    this->handle = CHK(SDL_CreateWindow(title, w, h, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN));
    CHK(SDL_GetWindowSizeInPixels(this->handle, &this->w, &this->h));
    CHK(SDL_Vulkan_CreateSurface(this->handle, vk::instance, nullptr, &surface));
}

void Window::destroy() {
    vkDestroySurfaceKHR(vk::instance, this->surface, nullptr);
    SDL_DestroyWindow(this->handle);
}

Window::operator SDL_Window*() const {
    return this->handle;
}

void Window::poll_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                should_close = true;
                break;

            case SDL_EVENT_WINDOW_RESIZED:
                w = event.window.data1;
                h = event.window.data2;
                break;
        }
    }
}