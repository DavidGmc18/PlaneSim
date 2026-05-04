#pragma once

#include <glm/glm.hpp>
#include <vector>

// TODO remove norm (union with tan)
struct Vertex {
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 uv;
    glm::vec4 tan;

    Vertex() = default;

    Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v):
        pos(x, y, z), norm(nx, ny, nz), uv(u, v), tan(0.0f) {}
};

inline void compute_tangents(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    std::vector<glm::vec3> tan_accum(vertices.size(), glm::vec3(0.0f));
    std::vector<glm::vec3> bitan_accum(vertices.size(), glm::vec3(0.0f));

    for (size_t i = 0; i < indices.size(); i += 3) {
        Vertex& v1 = vertices[indices[i]];
        Vertex& v2 = vertices[indices[i+1]];
        Vertex& v3 = vertices[indices[i+2]];

        glm::vec3 edge1 = v2.pos - v1.pos;
        glm::vec3 edge2 = v3.pos - v1.pos;
        glm::vec2 duv1 = v2.uv - v1.uv;
        glm::vec2 duv2 = v3.uv - v1.uv;

        float f = 1.0f / (duv1.x * duv2.y - duv2.x * duv1.y);

        glm::vec3 tangent = f * glm::vec3(
            duv2.y * edge1.x - duv1.y * edge2.x,
            duv2.y * edge1.y - duv1.y * edge2.y,
            duv2.y * edge1.z - duv1.y * edge2.z
        );

        glm::vec3 bitangent = f * glm::vec3(
            -duv2.x * edge1.x + duv1.x * edge2.x,
            -duv2.x * edge1.y + duv1.x * edge2.y,
            -duv2.x * edge1.z + duv1.x * edge2.z
        );

        tan_accum[indices[i]] += tangent;
        tan_accum[indices[i+1]] += tangent;
        tan_accum[indices[i+2]] += tangent;

        bitan_accum[indices[i]] += bitangent;
        bitan_accum[indices[i+1]] += bitangent;
        bitan_accum[indices[i+2]] += bitangent;
    }

    for (size_t i = 0; i < vertices.size(); i++) {
        glm::vec3 N = vertices[i].norm;
        glm::vec3 T = tan_accum[i];
        T = glm::normalize(T - glm::dot(T, N) * N);
        float w = glm::dot(glm::cross(N, T), bitan_accum[i]) < 0.0f ? -1.0f : 1.0f;
        vertices[i].tan = glm::vec4(T, w);
    }
}