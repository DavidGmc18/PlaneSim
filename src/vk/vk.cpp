#include "vk.hpp"
#include <SDL3/SDL_vulkan.h>

VkInstance vk::instance = VK_NULL_HANDLE;

void vk::init() {
    // Instance creation
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Sim",
        .apiVersion = VK_API_VERSION_1_3
    };

    u32 instance_extension_count = 0;
    const char* const* instance_extensions = SDL_Vulkan_GetInstanceExtensions(&instance_extension_count);

    VkInstanceCreateInfo instance_ci = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
        .enabledExtensionCount = instance_extension_count,
        .ppEnabledExtensionNames = instance_extensions
    };

    CHK(vkCreateInstance(&instance_ci, nullptr, &vk::instance));
}

void vk::destroy() {
    vkDestroyInstance(vk::instance, nullptr);
}