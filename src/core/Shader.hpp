#pragma once

#include "types.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <glm/glm.hpp>

struct ShaderDataBuffer {
    VmaAllocation allocation = VK_NULL_HANDLE;
    VmaAllocationInfo allocation_info = {};
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceAddress device_address = {};
};

struct ShaderData {
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model[3];
    glm::vec4 light_pos = {0.0f, -10.0f, 10.0f, 0.0f};
    u32 selected = 1;
};