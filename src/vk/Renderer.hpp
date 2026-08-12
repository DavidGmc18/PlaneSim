#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Device.hpp"
#include "core/Window.hpp"

namespace vk {
    struct Renderer {
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        VkExtent2D extent = {};
        std::vector<VkImage> images = {};
        std::vector<VkImageView> views = {};

        Renderer(const vk::Device& device, const Window& window);
        void destroy(const vk::Device& device);
    };
}