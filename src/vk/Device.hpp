#pragma once

#include "types.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace vk {
    struct Device {
        VkPhysicalDevice physical_device = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;

        u32 queue_family = U32_MAX;
        VkQueue queue = {};

        VmaAllocator allocator = VK_NULL_HANDLE;

        Device(VkSurfaceKHR surface);
        void destroy();

        operator VkPhysicalDevice() const;
        operator VkDevice() const;
    };
}