#pragma once

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 uv;

    Vertex() = default;

    Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v):
        pos(x, y, z), norm(nx, ny, nz), uv(u, v) {}
};