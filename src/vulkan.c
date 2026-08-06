#include "vulkan.h"

#include "types.h"

#include <stdlib.h>
#include <stdio.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

static inline VkResult chk_vk(VkResult res, const char* expr, const char* file, int line) {
    if (res != VK_SUCCESS) {
        fprintf(stderr, "VK ERROR %d (%s) at %s:%d\n", res, expr, file, line);
        exit(res);
    }
    return res;
}

static inline bool chk_bool(bool res, const char* expr, const char* file, int line) {
    if (!res) {
        fprintf(stderr, "ERROR (%s) at %s:%d\n", expr, file, line);
        exit(-1);
    }
    return res;
}

// TODO remove if this is unused
static inline void* chk_ptr(void* res, const char* expr, const char* file, int line) {
    if (!res) {
        fprintf(stderr, "NULL pointer (%s) at %s:%d\n", expr, file, line);
        exit(-1);
    }
    return res;
}

#define chk(x) _Generic((x), \
    VkResult: chk_vk, \
    bool: chk_bool, \
    void*: chk_ptr \
)(x, #x, __FILE__, __LINE__)

VkInstance instance = VK_NULL_HANDLE;
VkDevice device = VK_NULL_HANDLE;
// VkQueue queue = VK_NULL_HANDLE;
// VkSurfaceKHR surface = VK_NULL_HANDLE;

static inline void create_instance() {
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Sim",
        .apiVersion = VK_API_VERSION_1_3
    };

    u32 instance_extension_count = 0;
    const char* const* instance_extensions = SDL_Vulkan_GetInstanceExtensions(&instance_extension_count);

    VkInstanceCreateInfo instance_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
        .enabledExtensionCount = instance_extension_count,
        .ppEnabledExtensionNames = instance_extensions
    };

    chk(vkCreateInstance(&instance_info, NULL, &instance));
}

static inline void create_device() {
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

    VkPhysicalDevice physical_device = devices[best_device];

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physical_device, &properties);
    printf("Selected device: %s\n", properties.deviceName);
}

void vulkan_init() {
    chk(SDL_Init(SDL_INIT_VIDEO)); // TODO move into SDL module

    create_instance();
    create_device();
}