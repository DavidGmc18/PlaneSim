#include "common.hpp"
#include "core/Window.hpp"
#include "types.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_vulkan.h>
#include <cstdio>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "vk/vk.hpp"
#include "vk/Device.hpp"
#include "vk/Renderer.hpp"
#include <chrono>

constexpr u32 max_frames_in_flight = 2;

VkFence fences[max_frames_in_flight];
VkSemaphore image_acquired_semaphores[max_frames_in_flight];
VkSemaphore render_complete_semaphores[max_frames_in_flight];

VkCommandPool command_pool = VK_NULL_HANDLE;
VkCommandBuffer command_buffers[max_frames_in_flight];

int main() {
    CHK(SDL_Init(SDL_INIT_VIDEO));
    vk::init();

    Window window("Sim", 1280, 720);
    vk::Device device(window.surface);
    vk::Renderer renderer(device, window);




    VkFenceCreateInfo fence_create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };
    VkSemaphoreCreateInfo semaphore_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };
    for (u32 i = 0; i < max_frames_in_flight; i++) {
        CHK(vkCreateFence(device, &fence_create_info, nullptr, &fences[i]));
        CHK(vkCreateSemaphore(device, &semaphore_create_info, nullptr, &image_acquired_semaphores[i]));
        CHK(vkCreateSemaphore(device, &semaphore_create_info, nullptr, &render_complete_semaphores[i]));
    }

    VkCommandPoolCreateInfo command_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = device.queue_family
    };
    CHK(vkCreateCommandPool(device, &command_pool_create_info, nullptr, &command_pool));

    VkCommandBufferAllocateInfo command_buffer_alloc_create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = command_pool,
        .commandBufferCount = max_frames_in_flight
    };
    CHK(vkAllocateCommandBuffers(device, &command_buffer_alloc_create_info, command_buffers));





    std::chrono::time_point last = std::chrono::steady_clock::now();

    u32 frame = 0;
    while (!window.should_close) {
        window.poll_events();

        VkFence current_fence = fences[frame];

        // Wait for fence
        CHK(vkWaitForFences(device, 1, &current_fence, VK_TRUE, U64_MAX));
        CHK(vkResetFences(device, 1, &current_fence));

        // Get swapchain image
        u32 image_index;
        CHK(vkAcquireNextImageKHR(device, renderer.swapchain, U64_MAX, image_acquired_semaphores[frame], VK_NULL_HANDLE, &image_index));

        // Begin CMD
        VkCommandBuffer cmd = command_buffers[frame];
        CHK(vkResetCommandBuffer(cmd, 0));
        VkCommandBufferBeginInfo cmd_bi = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
        };
        CHK(vkBeginCommandBuffer(cmd, &cmd_bi));

        VkImageMemoryBarrier2 to_color = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_NONE,
            .srcAccessMask = VK_ACCESS_2_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .image = renderer.images[image_index],
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };
        VkDependencyInfo dependency_info = {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &to_color
        };
        vkCmdPipelineBarrier2(cmd, &dependency_info);

        // Dynamic rendering
        VkClearValue clear_value = {
            .color = { {0.05f, 0.10f, 0.20f, 1.0f} }
        };
        VkRenderingAttachmentInfo color_attachment = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = renderer.views[image_index],
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = clear_value
        };
        VkRenderingInfo rendering_info = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea = {
                .offset = { 0, 0 },
                .extent = renderer.extent
            },
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &color_attachment
        };

        vkCmdBeginRendering(cmd, &rendering_info);

        // Nothing to draw yet.
        // The image is simply cleared.
        vkCmdEndRendering(cmd);

        VkImageMemoryBarrier2 to_present = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_NONE,
            .dstAccessMask = VK_ACCESS_2_NONE,
            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .image = renderer.images[image_index],
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };
        dependency_info.pImageMemoryBarriers = &to_present;
        vkCmdPipelineBarrier2(cmd, &dependency_info);

        CHK(vkEndCommandBuffer(cmd));

        // Submit
        VkCommandBufferSubmitInfo command_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            .commandBuffer = cmd
        };
        VkSemaphoreSubmitInfo wait_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = image_acquired_semaphores[frame],
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
        };
        VkSemaphoreSubmitInfo signal_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = render_complete_semaphores[image_index],
            .stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT
        };
        VkSubmitInfo2 submit_info = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount = 1,
            .pWaitSemaphoreInfos = &wait_info,
            .commandBufferInfoCount = 1,
            .pCommandBufferInfos = &command_info,
            .signalSemaphoreInfoCount = 1,
            .pSignalSemaphoreInfos = &signal_info
        };
        CHK(vkQueueSubmit2(device.queue, 1, &submit_info, current_fence));

        // Present
        VkPresentInfoKHR present_info = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &render_complete_semaphores[image_index],
            .swapchainCount = 1,
            .pSwapchains = &renderer.swapchain,
            .pImageIndices = &image_index
        };
        CHK(vkQueuePresentKHR(device.queue, &present_info));

        frame = (frame + 1) % max_frames_in_flight;

        std::chrono::time_point now = std::chrono::steady_clock::now();
        double frame_time = std::chrono::duration<double, std::milli>(now - last).count();
        printf("%7.3f ms\n", frame_time);
        last = now;
    }







    vkDestroyCommandPool(device, command_pool, nullptr);

    for (u32 i = 0; i < max_frames_in_flight; i++) {
        vkDestroyFence(device, fences[i], nullptr);
		vkDestroySemaphore(device, image_acquired_semaphores[i], nullptr);
        vkDestroySemaphore(device, render_complete_semaphores[i], nullptr);
    }

    renderer.destroy(device);
    device.destroy();
    window.destroy();

    vk::destroy();
    SDL_Quit();
    return 0;
}