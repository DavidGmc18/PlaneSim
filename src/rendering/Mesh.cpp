#include "Mesh.hpp"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

Mesh::Mesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

Mesh::~Mesh() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Triangle> triangles, Material material): Mesh() {
    this->vertices = std::move(vertices);
    this->triangles = std::move(triangles);
    this->material = material;
    build();
}

Mesh::Mesh(Mesh&& other) noexcept
    : VBO(other.VBO), VAO(other.VAO), EBO(other.EBO), index_count(other.index_count),
    vertices(std::move(other.vertices)), 
    triangles(std::move(other.triangles)),
    material(other.material)
{
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);

        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        index_count = other.index_count;
        
        vertices = std::move(other.vertices);
        triangles = std::move(other.triangles);
        material = other.material;

        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
        other.index_count = 0;
    }
    return *this;
}

void Mesh::build() {
    if (vertices.empty() || triangles.empty()) return;

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);  

    index_count = triangles.size() * 3;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned int), triangles.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, norm));
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(3);	
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tan));

    glBindVertexArray(0);
}

void Mesh::render(GLuint shader, const glm::dmat4& double_model, const glm::dmat4& double_view, const glm::mat4& projection) const {
    material.use(shader);

    glm::mat4 mv = glm::mat4(double_view * double_model);
    glm::mat4 mvp = projection * mv;

    glm::mat4 model = glm::mat4(double_model);
    glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(model)));

    glUniformMatrix4fv(glGetUniformLocation(shader, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix3fv(glGetUniformLocation(shader, "uNormalMatrix"), 1, GL_FALSE, glm::value_ptr(normal_matrix));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
}