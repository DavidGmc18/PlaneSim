cmake_minimum_required(VERSION 3.12)
project(PlaneSim LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(assimp REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp 
    src/glad/glad.c
    src/shader.hpp
    src/stb/stb_image.h
    src/shader.hpp
    src/model.hpp
    src/light.hpp
    src/material.hpp
    src/TextureCache.hpp
    src/TextureCache.cpp
    src/world/World.hpp
    src/world/TerrainGenerator.hpp
    src/world/TerrainGenerator.cpp
    src/world/World.cpp
    src/FastNoiseLite/FastNoiseLite.h
    src/Mesh.hpp
    src/Mesh.cpp
    src/vehicle/RigidBody.hpp
    src/vehicle/RigidBody.cpp
    src/CONSTANTS.h
    src/vehicle/Aircraft.hpp
    src/vehicle/Aircraft.cpp
    src/vehicle/Wing.hpp
    src/vehicle/Wing.cpp
    src/vehicle/Engine.hpp
    src/vehicle/Engine.cpp
    src/vehicle/F16.hpp
    src/vehicle/F16.cpp
    src/vehicle/Hitbox.hpp
    src/vehicle/Hitbox.cpp
    src/Vertex.hpp
    src/Vertex.cpp
    src/Camera.hpp
    src/Camera.cpp
    src/stb/stb_truetype.h
    src/TextRenderer.hpp
    src/TextRenderer.cpp
    src/KeyHandler.hpp
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

add_definitions(-DGL_MAJOR=4)
add_definitions(-DGL_MINOR=6)