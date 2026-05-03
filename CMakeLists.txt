cmake_minimum_required(VERSION 3.12)
project(PlaneSim LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(assimp REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp 
    src/glad/glad.c
    src/shader.hpp
    src/stb/stb_image.h
    src/camera.hpp
    src/mesh.hpp
    src/shader.hpp
    src/model.hpp
    src/light.hpp
    src/material.hpp
    src/vertex.hpp
    src/TextureCache.hpp
    src/TextureCache.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    OpenGL::GL 
    SDL2::SDL2
    assimp::assimp
)

if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa" "-framework IOKit")
endif()

add_definitions(-DSHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/")
add_definitions(-DGL_MAJOR=4)
add_definitions(-DGL_MINOR=6)