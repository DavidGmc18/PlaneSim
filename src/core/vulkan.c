#include "vulkan.h"

#include "types.h"

#include <SDL3/SDL_video.h>
#include <stdlib.h>
#include <stdio.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <vk_mem_alloc.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*(arr)))

static inline bool chk_bool(bool res, const char* expr, const char* file, int line) {
    if (!res) {
        fprintf(stderr, "ERROR (%s) at %s:%d\n", expr, file, line);
        exit(-1);
    }
    return res;
}

static inline void* chk_ptr(void* res, const char* expr, const char* file, int line) {
    if (!res) {
        fprintf(stderr, "NULL pointer (%s) at %s:%d\n", expr, file, line);
        exit(-1);
    }
    return res;
}

static inline VkResult chk_vk(VkResult res, const char* expr, const char* file, int line) {
    if (res != VK_SUCCESS) {
        fprintf(stderr, "VK ERROR %d (%s) at %s:%d\n", res, expr, file, line);
        exit(res);
    }
    return res;
}

#define chk(x) _Generic((x), \
    bool: chk_bool, \
    void*: chk_ptr, \
    VkResult: chk_vk \
)(x, #x, __FILE__, __LINE__)

SDL_Window* window = NULL;
VkInstance instance = VK_NULL_HANDLE;
VkSurfaceKHR surface = VK_NULL_HANDLE;
VkDevice device = VK_NULL_HANDLE;
VmaAllocator allocator = VK_NULL_HANDLE;
VkSwapchainKHR swapchain = VK_NULL_HANDLE;

u32 swapchain_image_count = 0;
VkImage* swapchain_images = NULL;

VkImage depth_image;
VmaAllocation depth_image_allocation;
VkImageView depth_image_view;

static inline void create_instance() {
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

    chk(vkCreateInstance(&instance_create_info, NULL, &instance));
}

static inline VkPhysicalDevice select_physical_device() {
    u32 device_count = 0;
    chk(vkEnumeratePhysicalDevices(instance, &device_count, NULL));
    VkPhysicalDevice devices[device_count];
    chk(vkEnumeratePhysicalDevices(instance, &device_count, devices));

    u64 best_score = 0;
    u32 best_device = 0;

    for (u32 i = 0; i < device_count; i++) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(devices[i], &properties);
        VkPhysicalDeviceMemoryProperties memory;
        vkGetPhysicalDeviceMemoryProperties(devices[i], &memory);

        u64 score = 0;
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += (u64)1<<50; 
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) score += (u64)1<<40;

        for (u32 j = 0; j < memory.memoryHeapCount; j++)
            if (memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) score += memory.memoryHeaps[j].size;

        if (score > best_score) {
            best_score = score;
            best_device = i;
        }
    }

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(devices[best_device], &properties);
    printf("Selected device: %s\n", properties.deviceName);

    return devices[best_device];
}

static inline u32 find_queue_family(VkPhysicalDevice physical_device) {
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, NULL);
    VkQueueFamilyProperties queue_families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families);

    for (u32 i = 0; i < queue_family_count; i++) {
        VkBool32 present_support = VK_FALSE;
        chk(vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support));
        if ((queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT ) && present_support)
            return i;
    }

    fprintf(stderr, "Failed to find queue family!\n");
    exit(-1);
}

static inline void create_device(VkPhysicalDevice physical_device, u32 queue_family) {
    const float q_priorities =1.0f;
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

    chk(vkCreateDevice(physical_device, &device_create_info, NULL, &device));
}

static inline void create_allocator(VkPhysicalDevice physical_device) {
    VmaVulkanFunctions vk_functions = {
        .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
        .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
        .vkCreateImage = vkCreateImage
    };
    VmaAllocatorCreateInfo allocator_create_info = {
        .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT, 
        .physicalDevice = physical_device,
        .device = device,
        .pVulkanFunctions = &vk_functions,
        .instance = instance
    };
    chk(vmaCreateAllocator(&allocator_create_info, &allocator));
}

static inline void create_swapchain(VkPhysicalDevice physical_device) {
    VkSurfaceCapabilitiesKHR surface_caps;
    chk(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_caps));

    int window_width, window_height;
    chk(SDL_GetWindowSize(window, &window_width, &window_height));

    VkExtent2D swapchain_extent = surface_caps.currentExtent;
    if (surface_caps.currentExtent.width == 0xFFFFFFFF) {
        swapchain_extent.width = window_width;
        swapchain_extent.height = window_height;
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

    chk(vkCreateSwapchainKHR(device, &swapchain_create_info, NULL, &swapchain));

    chk(vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, NULL));
    swapchain_images = (VkImage*)chk(malloc(swapchain_image_count * sizeof(VkImage)));
    chk(vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, swapchain_images));

    VkFormat depth_formats[] = { VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
    VkFormat depth_format = VK_FORMAT_UNDEFINED;
    for (u32 i = 0; i < ARRAY_SIZE(depth_formats); i++) {
        VkFormatProperties2 format_properties = { .sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2 };
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
            .width = (u32)window_width,
            .height = (u32)window_height,
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

    chk(vmaCreateImage(allocator, &depth_image_create_info, &alloc_create_info, &depth_image, &depth_image_allocation, NULL));

    VkImageViewCreateInfo depth_view_create_info = { 
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = depth_image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = depth_format,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
            .levelCount = 1,
            .layerCount = 1
        }
    };
    
    chk(vkCreateImageView(device, &depth_view_create_info, NULL, &depth_image_view));
}

void vulkan_init() {
    chk(SDL_Init(SDL_INIT_VIDEO));
    window = SDL_CreateWindow("Sim", 1280, 720, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    create_instance();

    chk(SDL_Vulkan_CreateSurface(window, instance, NULL, &surface));

    VkPhysicalDevice physical_device = select_physical_device();
    u32 queue_family = find_queue_family(physical_device);
    create_device(physical_device, queue_family);

    create_allocator(physical_device);

    create_swapchain(physical_device);
}

void vulkan_destroy() {
    free(swapchain_images);
    swapchain_image_count = 0;

    // depth_image_allocation
}