#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

struct Triangle {
    unsigned int v0, v1, v2;

    Triangle() = default;
    Triangle(unsigned int v0, unsigned int v1, unsigned int v2);
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 uv;
    glm::vec4 tan;

    Vertex() = default;
    Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v);

    friend std::ostream& operator<<(std::ostream& os, const Vertex& v);

    static void compute_normals(std::vector<Vertex>& vertices, const std::vector<Triangle>& indices);
    static void compute_tangents(std::vector<Vertex>& vertices, const std::vector<Triangle>& indices);
};