#include "Vulkan.hpp"
#include "types.h"
#include <SDL3/SDL_vulkan.h>
#include <vector>
#include <vulkan/vulkan_core.h>

VkInstance vk::instance = VK_NULL_HANDLE;
VkPhysicalDevice vk::physical_device = VK_NULL_HANDLE;

void vk::init() {
    // Instance creation
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Sim",
        .apiVersion = VK_API_VERSION_1_3
    };

    u32 instance_extension_count = 0;
    const char* const* instance_extensions = SDL_Vulkan_GetInstanceExtensions(&instance_extension_count);

    VkInstanceCreateInfo instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
        .enabledExtensionCount = instance_extension_count,
        .ppEnabledExtensionNames = instance_extensions
    };

    CHK(vkCreateInstance(&instance_create_info, nullptr, &instance));


    // Physical device selection
    std::vector<VkPhysicalDevice> devices = vkGet<VkPhysicalDevice>(vkEnumeratePhysicalDevices, instance);

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
            physical_device = device;
        }
    }
    CHK(physical_device);

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physical_device, &properties);
    printf("Selected device: %s\n", properties.deviceName);
}

void vk::destroy() {
    vkDestroyInstance(instance, nullptr);
}

vk::Device::Device(VkSurfaceKHR surface) {
    std::vector<VkQueueFamilyProperties> family_properties = vkGet<VkQueueFamilyProperties>(vkGetPhysicalDeviceQueueFamilyProperties, physical_device);
    for (usize i = 0; i < family_properties.size(); i++) {
        VkBool32 present_support = VK_FALSE;
        CHK(vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support));
        if ((family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && present_support) {
            this->queue_family = i;
            break;
        }
    }

    if (this->queue_family == U32_MAX) {
        fprintf(stderr, "Failed to find queue family!\n");
        exit(-1);
    }

    const float q_priorities = 1.0f;
    VkDeviceQueueCreateInfo queue_create_info = {
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

    VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &enabled_vk_1_3_features,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_create_info,
        .enabledExtensionCount = (u32)ARRAY_SIZE(device_extensions),
        .ppEnabledExtensionNames = device_extensions,
        .pEnabledFeatures = &enabled_vk_1_0_features
    };

    CHK(vkCreateDevice(physical_device, &device_create_info, nullptr, &this->handle));

    // Allocator creation
    VmaVulkanFunctions vk_functions = {
        .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
        .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
        .vkCreateImage = vkCreateImage
    };

    VmaAllocatorCreateInfo allocator_create_info = {
        .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT, 
        .physicalDevice = physical_device,
        .device = this->handle,
        .pVulkanFunctions = &vk_functions,
        .instance = instance
    };

    CHK(vmaCreateAllocator(&allocator_create_info, &this->allocator));
}

void vk::Device::destroy() {
    vmaDestroyAllocator(this->allocator);
    vkDestroyDevice(this->handle, nullptr);
}

vk::Renderer::Renderer(VkSurfaceKHR surface) {
    this->surface = surface;
}

void vk::Renderer::create(const vk::Device& device, const Window& window) {
    VkSurfaceCapabilitiesKHR surface_caps;
    CHK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_caps));

    VkExtent2D swapchain_extent = surface_caps.currentExtent;
    if (surface_caps.currentExtent.width == 0xFFFFFFFF) {
        swapchain_extent.width = window.w;
        swapchain_extent.height = window.h;
    }

    const VkFormat imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
    VkSwapchainCreateInfoKHR swapchain_create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = surface_caps.minImageCount,
        .imageFormat = imageFormat,
        .imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
        .imageExtent = {
            .width = swapchain_extent.width,
            .height = swapchain_extent.height
        },
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR
    };

    CHK(vkCreateSwapchainKHR(device.handle, &swapchain_create_info, nullptr, &swapchain));

    // Images
    swapchain_images = vkGet<VkImage>(vkGetSwapchainImagesKHR, device.handle, swapchain);

    VkFormat depth_formats[] = { VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
    VkFormat depth_format = VK_FORMAT_UNDEFINED;
    for (u32 i = 0; i < ARRAY_SIZE(depth_formats); i++) {
        VkFormatProperties2 format_properties = {.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2 };
        vkGetPhysicalDeviceFormatProperties2(physical_device, depth_formats[i], &format_properties);
        if (format_properties.formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            depth_format = depth_formats[i];
            break;
        }
    }

    VkImageCreateInfo depth_image_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = depth_format,
        .extent = {
            .width = (u32)window.w,
            .height = (u32)window.h,
            .depth = 1
        },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VmaAllocationCreateInfo alloc_create_info = {
        .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO
    };

    CHK(vmaCreateImage(device.allocator, &depth_image_create_info, &alloc_create_info, &this->depth_image, &this->depth_image_allocation, NULL));

    VkImageViewCreateInfo depth_view_create_info = { 
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = this->depth_image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = depth_format,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
            .levelCount = 1,
            .layerCount = 1
        }
    };
    
    CHK(vkCreateImageView(device.handle, &depth_view_create_info, NULL, &this->depth_image_view));
}

void vk::Renderer::destroy(const vk::Device& device) {
    vkDestroyImageView(device.handle, this->depth_image_view, nullptr);
    vmaDestroyImage(device.allocator, this->depth_image, this->depth_image_allocation);
    vkDestroySwapchainKHR(device.handle, this->swapchain, nullptr);
    vkDestroySurfaceKHR(instance, this->surface, nullptr);
}