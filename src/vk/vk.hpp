#pragma once

#include <type_traits>
#include <vector>

#include "types.h"
#include "common.hpp"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

inline VkResult chk(VkResult res, const char* expr, const char* file, int line) {
    if (res != VK_SUCCESS) {
        fprintf(stderr, "VK ERROR %d (%s) at %s:%d\n", res, expr, file, line);
        exit(res);
    }
    return res;
}

template<typename T, typename F, typename... Args>
requires (!std::is_void_v<std::invoke_result_t<F, Args..., u32*, T*>>)
inline std::vector<T> vkGet(F&& f, Args&&... args) {
    u32 count = 0;
    CHK(f(std::forward<Args>(args)..., &count, nullptr));
    std::vector<T> vec(count);
    if (count > 0) CHK(f(std::forward<Args>(args)..., &count, vec.data()));
    return vec;
}

template<typename T, typename F, typename... Args>
requires (std::is_void_v<std::invoke_result_t<F, Args..., u32*, T*>>)
inline std::vector<T> vkGet(F&& f, Args&&... args) {
    u32 count = 0;
    f(std::forward<Args>(args)..., &count, nullptr);
    std::vector<T> vec(count);
    if (count > 0) f(std::forward<Args>(args)..., &count, vec.data());
    return vec;
}

namespace vk {
    extern VkInstance instance;

    void init();
    void destroy();
}