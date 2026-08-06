cmake_minimum_required(VERSION 3.20)

project(sim LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
    src/*.c
)

add_executable(sim ${SOURCES})

target_compile_options(sim PRIVATE
    -O2
    -g3
    -Wall
    -Wextra
    -Wno-missing-field-initializers
)

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED CONFIG)
target_link_libraries(sim PRIVATE
    Vulkan::Vulkan
    SDL3::SDL3
)