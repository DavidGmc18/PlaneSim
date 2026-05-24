#pragma once

#include <vector>
#include "Vertex.hpp"
#include "Material.hpp"
#include <glad/glad.h>

class Mesh {
    GLuint VBO = 0;
    GLuint VAO = 0;
    GLuint EBO = 0;
    unsigned int index_count = 0;

public:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    Material material;

    Mesh();
    ~Mesh();

    Mesh(std::vector<Vertex> vertices, std::vector<Triangle> triangles, Material material);

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void build();
    
    void render(GLuint shader, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const;
};