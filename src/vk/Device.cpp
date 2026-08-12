#include "Device.hpp"
#include "vk.hpp"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

vk::Device::Device(VkSurfaceKHR surface) {
    // Physical device selection
    std::vector<VkPhysicalDevice> devices = vkGet<VkPhysicalDevice>(vkEnumeratePhysicalDevices, vk::instance);

    u64 best_score = 0;
    for (const VkPhysicalDevice& device : devices) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);
        VkPhysicalDeviceMemoryProperties memory;
        vkGetPhysicalDeviceMemoryProperties(device, &memory);

        u64 score = 0;
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += (u64)1<<50; 
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) score += (u64)1<<40;

        for (u32 j = 0; j < memory.memoryHeapCount; j++)
            if (memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) score += memory.memoryHeaps[j].size;

        if (score > best_score) {
            best_score = score;
            this->physical_device = device;
        }
    }
    CHK(this->physical_device);

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(this->physical_device, &properties);
    printf("Selected device: %s\n", properties.deviceName);

    // Family selection
    std::vector<VkQueueFamilyProperties> family_properties = vkGet<VkQueueFamilyProperties>(vkGetPhysicalDeviceQueueFamilyProperties, this->physical_device);
    for (usize i = 0; i < family_properties.size(); i++) {
        VkBool32 present_support = VK_FALSE;
        CHK(vkGetPhysicalDeviceSurfaceSupportKHR(this->physical_device, i, surface, &present_support));
        if ((family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && present_support) {
            this->queue_family = i;
            break;
        }
    }

    if (this->queue_family == U32_MAX) {
        fprintf(stderr, "Failed to find queue family!\n");
        exit(-1);
    }

    // Create device
    const float q_priorities = 1.0f;
    VkDeviceQueueCreateInfo queue_ci = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queue_family,
        .queueCount = 1,
        .pQueuePriorities = &q_priorities
    };

    const char* device_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    VkPhysicalDeviceVulkan12Features enabled_vk_1_2_features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .descriptorIndexing = true,
        .shaderSampledImageArrayNonUniformIndexing = true,
        .descriptorBindingVariableDescriptorCount = true,
        .runtimeDescriptorArray = true,
        .bufferDeviceAddress = true
    };

    VkPhysicalDeviceVulkan13Features enabled_vk_1_3_features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = &enabled_vk_1_2_features,
        .synchronization2 = true,
        .dynamicRendering = true,
    };

    VkPhysicalDeviceFeatures enabled_vk_1_0_features = {
        .samplerAnisotropy = VK_TRUE
    };

    VkDeviceCreateInfo device_ci = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &enabled_vk_1_3_features,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_ci,
        .enabledExtensionCount = (u32)ARRAY_SIZE(device_extensions),
        .ppEnabledExtensionNames = device_extensions,
        .pEnabledFeatures = &enabled_vk_1_0_features
    };

    CHK(vkCreateDevice(this->physical_device, &device_ci, nullptr, &this->device));

    // Get deivce queue
    vkGetDeviceQueue(this->device, this->queue_family, 0, &this->queue);

    // Create allocator
    VmaVulkanFunctions vk_functions = {
        .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
        .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
        .vkCreateImage = vkCreateImage
    };

    VmaAllocatorCreateInfo allocator_create_info = {
        .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT, 
        .physicalDevice = this->physical_device,
        .device = this->device,
        .pVulkanFunctions = &vk_functions,
        .instance = vk::instance
    };

    CHK(vmaCreateAllocator(&allocator_create_info, &this->allocator));
}

void vk::Device::destroy() {
    vmaDestroyAllocator(this->allocator);
    vkDestroyDevice(this->device, nullptr);
}

vk::Device::operator VkPhysicalDevice() const {
    return this->physical_device;
}

vk::Device::operator VkDevice() const {
    return this->device;
}