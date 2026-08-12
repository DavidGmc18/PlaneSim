#include "Renderer.hpp"
#include "vk.hpp"

vk::Renderer::Renderer(const vk::Device& device, const Window& window) {
    VkSurfaceCapabilitiesKHR surface_caps;
    CHK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, window.surface, &surface_caps));

    this->extent = surface_caps.currentExtent;
    if (surface_caps.currentExtent.width == 0xFFFFFFFF) {
        this->extent.width = window.w;
        this->extent.height = window.h;
    }

    const VkFormat image_format = VK_FORMAT_B8G8R8A8_SRGB;

    VkSwapchainCreateInfoKHR swapchain_ci = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = window.surface,
        .minImageCount = surface_caps.minImageCount,
        .imageFormat = image_format,
        .imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
        .imageExtent = {
            .width = this->extent.width,
            .height = this->extent.height
        },
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR
    };

    CHK(vkCreateSwapchainKHR(device, &swapchain_ci, nullptr, &this->swapchain));

    // Images
    this->images = vkGet<VkImage>(vkGetSwapchainImagesKHR, device, swapchain);

    // Views
    this->views.resize(this->images.size());
    for (usize i = 0; i < this->images.size(); i++) {
        VkImageViewCreateInfo img_ci = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = this->images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = image_format,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };
        CHK(vkCreateImageView(device, &img_ci, nullptr, &this->views[i]));
    }

    // VkFormat depth_formats[] = { VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
    // VkFormat depth_format = VK_FORMAT_UNDEFINED;
    // for (u32 i = 0; i < ARRAY_SIZE(depth_formats); i++) {
    //     VkFormatProperties2 format_properties = {.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2 };
    //     vkGetPhysicalDeviceFormatProperties2(physical_device, depth_formats[i], &format_properties);
    //     if (format_properties.formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
    //         depth_format = depth_formats[i];
    //         break;
    //     }
    // }

    // VkImageCreateInfo depth_image_create_info = {
    //     .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    //     .imageType = VK_IMAGE_TYPE_2D,
    //     .format = depth_format,
    //     .extent = {
    //         .width = (u32)window.w,
    //         .height = (u32)window.h,
    //         .depth = 1
    //     },
    //     .mipLevels = 1,
    //     .arrayLayers = 1,
    //     .samples = VK_SAMPLE_COUNT_1_BIT,
    //     .tiling = VK_IMAGE_TILING_OPTIMAL,
    //     .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    //     .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    // };

    // VmaAllocationCreateInfo alloc_create_info = {
    //     .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
    //     .usage = VMA_MEMORY_USAGE_AUTO
    // };

    // CHK(vmaCreateImage(device.allocator, &depth_image_create_info, &alloc_create_info, &this->depth_image, &this->depth_image_allocation, NULL));

    // VkImageViewCreateInfo depth_view_create_info = { 
    //     .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    //     .image = this->depth_image,
    //     .viewType = VK_IMAGE_VIEW_TYPE_2D,
    //     .format = depth_format,
    //     .subresourceRange = {
    //         .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
    //         .levelCount = 1,
    //         .layerCount = 1
    //     }
    // };
    
    // CHK(vkCreateImageView(device.handle, &depth_view_create_info, NULL, &this->depth_image_view));
}

void vk::Renderer::destroy(const vk::Device& device) {
    // vkDestroyImageView(device.handle, this->depth_image_view, nullptr);
    // vmaDestroyImage(device.allocator, this->depth_image, this->depth_image_allocation);
    vkDestroySwapchainKHR(device, this->swapchain, nullptr);
}