#include "common.hpp"
#include "core/Shader.hpp"
#include "core/Vertex.hpp"
#include "core/Window.hpp"

#include "core/Vulkan.hpp"
#include "types.h"
#include <SDL3/SDL_vulkan.h>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <tiny_obj_loader.h>
#include <ktx.h>
#include <ktxvulkan.h>
#include <array>

constexpr u32 max_frames_in_flight = 2;

ShaderDataBuffer shader_data_buffers[max_frames_in_flight];

ShaderData shader_data = {};

VkFence fences[max_frames_in_flight];
VkSemaphore image_acquired_semaphores[max_frames_in_flight];

VkCommandPool command_pool = VK_NULL_HANDLE;
VkCommandBuffer command_buffers[max_frames_in_flight];

std::array<Texture, 3> textures;

VkDescriptorSetLayout descriptor_set_layout_tex = VK_NULL_HANDLE;
VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
VkDescriptorSet descriptor_set_tex = VK_NULL_HANDLE;

inline KTX_error_code chk(KTX_error_code res, const char* expr, const char* file, int line) {
    if (res != KTX_SUCCESS) {
        fprintf(stderr, "KTX ERROR %d (%s) at %s:%d\n", res, expr, file, line);
        exit(res);
    }
    return res;
}

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

    VkCommandPoolCreateInfo command_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = device.queue_family
    };
    CHK(vkCreateCommandPool(device.handle, &command_pool_create_info, nullptr, &command_pool));

    VkCommandBufferAllocateInfo command_buffer_alloc_create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = command_pool,
        .commandBufferCount = max_frames_in_flight
    };
    CHK(vkAllocateCommandBuffers(device.handle, &command_buffer_alloc_create_info, command_buffers));





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
    
    // Load tetxures
    std::vector<VkDescriptorImageInfo> texture_descriptors = {};
    for (usize i = 0; i < textures.size(); i++) {
        ktxTexture* ktx_tex = nullptr;
        std::string file_name = "assets/suzanne" + std::to_string(i) + ".ktx";
        CHK(ktxTexture_CreateFromNamedFile(file_name.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktx_tex));

        VkImageCreateInfo tex_img_create_info{
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = ktxTexture_GetVkFormat(ktx_tex),
            .extent = {
                .width = ktx_tex->baseWidth,
                .height = ktx_tex->baseHeight,
                .depth = 1
            },
            .mipLevels = ktx_tex->numLevels,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };
        VmaAllocationCreateInfo tex_img_alloc_create_info = { .usage = VMA_MEMORY_USAGE_AUTO };
        CHK(vmaCreateImage(device.allocator, &tex_img_create_info, &tex_img_alloc_create_info, &textures[i].image, &textures[i].allocation, nullptr));

        VkImageViewCreateInfo tex_view_crete_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = textures[i].image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = tex_img_create_info.format,
            .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = ktx_tex->numLevels, .layerCount = 1 }
        };
        CHK(vkCreateImageView(device.handle, &tex_view_crete_info, nullptr, &textures[i].view));

        VkBuffer img_src_buffer = {};
        VmaAllocation img_src_allocation = {};
        VkBufferCreateInfo img_src_buffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = (uint32_t)ktx_tex->dataSize,
            .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
        };
        VmaAllocationCreateInfo img_src_alloc_create_info = {
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO
        };
        VmaAllocationInfo img_src_alloc_info = {};
        CHK(vmaCreateBuffer(device.allocator, &img_src_buffer_create_info, &img_src_alloc_create_info, &img_src_buffer, &img_src_allocation, &img_src_alloc_info));
        memcpy(img_src_alloc_info.pMappedData, ktx_tex->pData, ktx_tex->dataSize);

        VkFenceCreateInfo fence_one_time_create_info = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO
        };
        VkFence fence_one_time = {};
        CHK(vkCreateFence(device.handle, &fence_one_time_create_info, nullptr, &fence_one_time));
        VkCommandBuffer cb_one_time = {};
        VkCommandBufferAllocateInfo cb_one_time_alloc_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = command_pool,
            .commandBufferCount = 1
        };
        CHK(vkAllocateCommandBuffers(device.handle, &cb_one_time_alloc_info, &cb_one_time));

        VkCommandBufferBeginInfo cb_one_time_begin_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
        };
        CHK(vkBeginCommandBuffer(cb_one_time, &cb_one_time_begin_info));
        VkImageMemoryBarrier2 barrier_tex_image{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_NONE,
            .srcAccessMask = VK_ACCESS_2_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            .dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .image = textures[i].image,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .levelCount = ktx_tex->numLevels,
                .layerCount = 1
            }
        };
        VkDependencyInfo barrier_tex_info = {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &barrier_tex_image
        };
        vkCmdPipelineBarrier2(cb_one_time, &barrier_tex_info);
        std::vector<VkBufferImageCopy> copy_regions = {};
        for (u32 j = 0; j < ktx_tex->numLevels; j++) {
            ktx_size_t mipOffset{0};
            CHK(ktxTexture_GetImageOffset(ktx_tex, j, 0, 0, &mipOffset));
            copy_regions.push_back({
                .bufferOffset = mipOffset,
                .imageSubresource = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = (u32)j,
                    .layerCount = 1
                },
                .imageExtent = {
                    .width = ktx_tex->baseWidth >> j,
                    .height = ktx_tex->baseHeight >> j,
                    .depth = 1
                }
            });
        }
        vkCmdCopyBufferToImage(cb_one_time, img_src_buffer, textures[i].image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, (u32)copy_regions.size(), copy_regions.data());
        VkImageMemoryBarrier2 barrier_tex_read = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT,
            .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
            .image = textures[i].image,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .levelCount = ktx_tex->numLevels,
                .layerCount = 1
            }
        };
        barrier_tex_info.pImageMemoryBarriers = &barrier_tex_read;
        vkCmdPipelineBarrier2(cb_one_time, &barrier_tex_info);
        CHK(vkEndCommandBuffer(cb_one_time));
        VkCommandBufferSubmitInfo cb_one_time_submit_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            .commandBuffer = cb_one_time
        };
        VkSubmitInfo2 one_time_submit_info = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .commandBufferInfoCount = 1,
            .pCommandBufferInfos = &cb_one_time_submit_info
        };
        CHK(vkQueueSubmit2(device.queue, 1, &one_time_submit_info, fence_one_time));
        CHK(vkWaitForFences(device.handle, 1, &fence_one_time, VK_TRUE, U64_MAX));

        VkSamplerCreateInfo sampler_create_info = {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .anisotropyEnable = VK_TRUE,
            .maxAnisotropy = 8.0f, // 8 is a widely supported value for max anisotropy
            .maxLod = (float)ktx_tex->numLevels,
        };
        CHK(vkCreateSampler(device.handle, &sampler_create_info, nullptr, &textures[i].sampler));

        ktxTexture_Destroy(ktx_tex);
        texture_descriptors.push_back({
            .sampler = textures[i].sampler,
            .imageView = textures[i].view,
            .imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL
        });
    }

    VkDescriptorBindingFlags desc_variable_flag = VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
    VkDescriptorSetLayoutBindingFlagsCreateInfo desc_binding_flags = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
        .bindingCount = 1,
        .pBindingFlags = &desc_variable_flag
    };
    VkDescriptorSetLayoutBinding desc_layout_binding_tex = {
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = (u32)textures.size(),
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
    };
    VkDescriptorSetLayoutCreateInfo desc_layout_tex_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = &desc_binding_flags,
        .bindingCount = 1,
        .pBindings = &desc_layout_binding_tex
    };
    CHK(vkCreateDescriptorSetLayout(device.handle, &desc_layout_tex_create_info, nullptr, &descriptor_set_layout_tex));

    VkDescriptorPoolSize poolSize{
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = (u32)textures.size()
    };
    VkDescriptorPoolCreateInfo desc_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = 1,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize
    };
    CHK(vkCreateDescriptorPool(device.handle, &desc_pool_create_info, nullptr, &descriptor_pool));

    u32 variable_desc_count = (u32)textures.size();
    VkDescriptorSetVariableDescriptorCountAllocateInfo variable_desc_count_alloc_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT,
        .descriptorSetCount = 1,
        .pDescriptorCounts = &variable_desc_count
    };
    VkDescriptorSetAllocateInfo tex_desc_set_alloc_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = &variable_desc_count_alloc_info,
        .descriptorPool = descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptor_set_layout_tex
    };
    CHK(vkAllocateDescriptorSets(device.handle, &tex_desc_set_alloc_info, &descriptor_set_tex));

    VkWriteDescriptorSet write_desc_set = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptor_set_tex,
        .dstBinding = 0,
        .descriptorCount = (u32)texture_descriptors.size(),
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 
        .pImageInfo = texture_descriptors.data()
    };
    vkUpdateDescriptorSets(device.handle, 1, &write_desc_set, 0, nullptr);





    vkDestroyCommandPool(device.handle, command_pool, nullptr);

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