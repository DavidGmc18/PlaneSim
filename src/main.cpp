#include "common.hpp"
#include "core/Shader.hpp"
#include "core/Vertex.hpp"
#include "core/Window.hpp"

#include "core/Vulkan.hpp"
#include <SDL3/SDL_vulkan.h>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <array>
#include <tiny_obj_loader.h>

constexpr u32 max_frames_in_flight = 2;

std::array<ShaderDataBuffer, max_frames_in_flight> shader_data_buffers;
std::array<VkCommandBuffer, max_frames_in_flight> command_buffers;

ShaderData shader_data = {};

std::array<VkFence, max_frames_in_flight> fences;
std::array<VkSemaphore, max_frames_in_flight> image_acquired_semaphores;

int main() {
    CHK(SDL_Init(SDL_INIT_VIDEO));
    Window window("Sim", 1280, 720);

    vk::init();

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    CHK(SDL_Vulkan_CreateSurface(window.handle, vk::instance, nullptr, &surface));
    vk::Renderer renderer(surface);

    vk::Device device(surface);

    renderer.create(device, window);

    for (u32 i = 0; i < max_frames_in_flight; i++) {
        VkBufferCreateInfo u_buffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = sizeof(ShaderData),
            .usage = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
        };
            VmaAllocationCreateInfo u_buffer_alloc_create_info = {
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO
        };
        CHK(vmaCreateBuffer(device.allocator, &u_buffer_create_info, &u_buffer_alloc_create_info, &shader_data_buffers[i].buffer, &shader_data_buffers[i].allocation, &shader_data_buffers[i].allocation_info));
        VkBufferDeviceAddressInfo u_buffer_device_address_info = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
            .buffer = shader_data_buffers[i].buffer
        };
        shader_data_buffers[i].device_address = vkGetBufferDeviceAddress(device.handle, &u_buffer_device_address_info);
    }

    VkFenceCreateInfo fence_create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };
    VkSemaphoreCreateInfo semaphore_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };
    for (u32 i = 0; i < max_frames_in_flight; i++) {
        CHK(vkCreateFence(device.handle, &fence_create_info, nullptr, &fences[i]));
        CHK(vkCreateSemaphore(device.handle, &semaphore_create_info, nullptr, &image_acquired_semaphores[i]));
    }
    std::vector<VkSemaphore> render_complete_semaphores(renderer.swapchain_images.size());
    for (VkSemaphore& semaphore : render_complete_semaphores) {
        CHK(vkCreateSemaphore(device.handle, &semaphore_create_info, nullptr, &semaphore));
    }





    // Mesh data
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    CHK(tinyobj::LoadObj(&attrib, &shapes, &materials, nullptr, nullptr, "assets/suzanne.obj"));

    std::vector<Vertex> vertices;
    std::vector<u16> indices;
    for (auto& index : shapes[0].mesh.indices) {
        vertices.push_back({
            .pos = {
                attrib.vertices[index.vertex_index * 3],
                attrib.vertices[index.vertex_index * 3 + 1],
                attrib.vertices[index.vertex_index * 3 + 2]
            },
            .norm = {
                attrib.normals[index.normal_index * 3],
                attrib.normals[index.normal_index * 3 + 1],
                attrib.normals[index.normal_index * 3 + 2]
            },
            .uv = {
                attrib.texcoords[index.texcoord_index * 2],
                attrib.texcoords[index.texcoord_index * 2 + 1]
            }
        });
        indices.push_back(indices.size());
    }

    VkDeviceSize v_buffer_size = sizeof(Vertex) * vertices.size();
    VkDeviceSize i_buffer_size = sizeof(u16) * indices.size();
    VkBufferCreateInfo buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = v_buffer_size + i_buffer_size,
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
    };
    VmaAllocationCreateInfo buffer_allocation_create_info = {
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO
    };
    VmaAllocationInfo buffer_allocation_info = {};
    VkBuffer buffer = VK_NULL_HANDLE;
    VmaAllocation buffer_allocation = VK_NULL_HANDLE;
    CHK(vmaCreateBuffer(device.allocator, &buffer_create_info, &buffer_allocation_create_info, &buffer, &buffer_allocation, &buffer_allocation_info));
    memcpy(buffer_allocation_info.pMappedData, vertices.data(), v_buffer_size);
    memcpy(((char*)buffer_allocation_info.pMappedData) + v_buffer_size, indices.data(), i_buffer_size);





    vmaDestroyBuffer(device.allocator, buffer, buffer_allocation);

    for (usize i = 0; i < render_complete_semaphores.size(); i++) {
		vkDestroySemaphore(device.handle, render_complete_semaphores[i], nullptr);
	}

    for (u32 i = 0; i < max_frames_in_flight; i++) {
        vkDestroyFence(device.handle, fences[i], nullptr);
		vkDestroySemaphore(device.handle, image_acquired_semaphores[i], nullptr);
    }

    for (u32 i = 0; i < max_frames_in_flight; i++) {
		vmaDestroyBuffer(device.allocator, shader_data_buffers[i].buffer, shader_data_buffers[i].allocation);
	}

    renderer.destroy(device);

    device.destroy();

    vk::destroy();
    return 0;
}