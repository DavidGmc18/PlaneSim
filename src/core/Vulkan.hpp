#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vk_mem_alloc.h>

#include "Window.hpp"
#include "types.h"
#include "common.hpp"
#include <utility>

inline VkResult chk(VkResult res, const char* expr, const char* file, int line) {
    if (res != VK_SUCCESS) {
        fprintf(stderr, "VK ERROR %d (%s) at %s:%d\n", res, expr, file, line);
        exit(res);
    }
    return res;
}

template<typename T, typename F, typename... Args>
requires (!std::is_void_v<std::invoke_result_t<F, Args..., u32*, T*>>)
inline std::vector<T> vkGet(F&& f, Args&&... args) {
    u32 count = 0;
    CHK(f(std::forward<Args>(args)..., &count, nullptr));
    std::vector<T> vec(count);
    if (count > 0) CHK(f(std::forward<Args>(args)..., &count, vec.data()));
    return vec;
}

template<typename T, typename F, typename... Args>
requires (std::is_void_v<std::invoke_result_t<F, Args..., u32*, T*>>)
inline std::vector<T> vkGet(F&& f, Args&&... args) {
    u32 count = 0;
    f(std::forward<Args>(args)..., &count, nullptr);
    std::vector<T> vec(count);
    if (count > 0) f(std::forward<Args>(args)..., &count, vec.data());
    return vec;
}

namespace vk {
    extern VkInstance instance;
    extern VkPhysicalDevice physical_device;

    void init();
    void destroy();

    struct Device {
        VkDevice handle = VK_NULL_HANDLE;
        VmaAllocator allocator = VK_NULL_HANDLE;
        u32 queue_family = U32_MAX;
        VkQueue queue;

        Device(VkSurfaceKHR surface);
        void destroy();
    };

    struct Renderer {
        VkSurfaceKHR surface;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;

        std::vector<VkImage> swapchain_images;
        VkImage depth_image = VK_NULL_HANDLE;
        VmaAllocation depth_image_allocation = VK_NULL_HANDLE;
        VkImageView depth_image_view = VK_NULL_HANDLE;

        Renderer(VkSurfaceKHR surface);
        void create(const vk::Device& device, const Window& window);
        void destroy(const vk::Device& device);
    };
}