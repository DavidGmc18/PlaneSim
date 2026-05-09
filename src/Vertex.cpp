#include "Vertex.hpp"

#include <glm/gtx/string_cast.hpp>

Triangle::Triangle(unsigned int v0, unsigned int v1, unsigned int v2)
    : v0(v0), v1(v1), v2(v2) {}


Vertex::Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v):
    pos(x, y, z), norm(nx, ny, nz), uv(u, v), tan(0.0f) {}

std::ostream& operator<<(std::ostream& os, const Vertex& v) {
    os << "Vertex(";
    os << "pos=" << glm::to_string(v.pos);
    os << ", norm=" << glm::to_string(v.norm);
    os << ", uv=" << glm::to_string(v.uv);
    os << ", tan=" << glm::to_string(v.tan);
    os << ')';
    return os;
}

void Vertex::compute_normals(std::vector<Vertex>& vertices, const std::vector<Triangle>& indices) {
    for (Vertex& vertex : vertices) 
        vertex.norm = glm::vec3(0.0f);

    for (size_t i = 0; i < indices.size(); i++) {
        Vertex& v1 = vertices[indices[i].v0];
        Vertex& v2 = vertices[indices[i].v1];
        Vertex& v3 = vertices[indices[i].v2];

        glm::vec3 edge1 = v2.pos - v1.pos;
        glm::vec3 edge2 = v3.pos - v1.pos;
        glm::vec3 faceNormal = glm::cross(edge1, edge2);

        v1.norm += faceNormal;
        v2.norm += faceNormal;
        v3.norm += faceNormal;
    }

    for (Vertex& vertex : vertices) {
        if (glm::length(vertex.norm) > 0.0f) {
            vertex.norm = glm::normalize(vertex.norm);
        } else {
            vertex.norm = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }
}

void Vertex::compute_tangents(std::vector<Vertex>& vertices, const std::vector<Triangle>& indices) {
    std::vector<glm::vec3> tan_accum(vertices.size(), glm::vec3(0.0f));
    std::vector<glm::vec3> bitan_accum(vertices.size(), glm::vec3(0.0f));

    for (size_t i = 0; i < indices.size(); i++) {
        Vertex& v1 = vertices[indices[i].v0];
        Vertex& v2 = vertices[indices[i].v1];
        Vertex& v3 = vertices[indices[i].v2];

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

        tan_accum[indices[i].v0] += tangent;
        tan_accum[indices[i].v1] += tangent;
        tan_accum[indices[i].v2] += tangent;

        bitan_accum[indices[i].v0] += bitangent;
        bitan_accum[indices[i].v1] += bitangent;
        bitan_accum[indices[i].v2] += bitangent;
    }

    for (size_t i = 0; i < vertices.size(); i++) {
        glm::vec3 N = vertices[i].norm;
        glm::vec3 T = tan_accum[i];
        T = glm::normalize(T - glm::dot(T, N) * N);
        float w = glm::dot(glm::cross(N, T), bitan_accum[i]) < 0.0f ? -1.0f : 1.0f;
        vertices[i].tan = glm::vec4(T, w);
    }
}