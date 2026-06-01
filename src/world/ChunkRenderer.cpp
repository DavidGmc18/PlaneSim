#include "ChunkRenderer.hpp"
#include "rendering/Shader.hpp"
#include <vector>
#include "Chunk.hpp"

void ChunkRenderer::init() {
    if (ChunkRenderer::shader == 0) {
        ChunkRenderer::shader = compile_shader_program("shaders/chunk.vert", "shaders/chunk.frag");

        glUseProgram(ChunkRenderer::shader);
        glUniform1i(glGetUniformLocation(ChunkRenderer::shader, "uTexture"), 0);
        glUseProgram(0);
    }

    if (ChunkRenderer::VBO_XZ == 0) {
        std::vector<glm::vec2> xz;
        xz.resize((Chunk::SIZE + 1) * (Chunk::SIZE + 1));
        for (int z = 0; z <= Chunk::SIZE; z++) {
            for (int x = 0; x <= Chunk::SIZE; x++) {
                xz[z * (Chunk::SIZE + 1) + x] = glm::vec2(x, z);
            }
        }

        glGenBuffers(1, &ChunkRenderer::VBO_XZ);
        glBindBuffer(GL_ARRAY_BUFFER, ChunkRenderer::VBO_XZ);
        glBufferData(GL_ARRAY_BUFFER, xz.size() * sizeof(glm::vec2), xz.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (ChunkRenderer::EBO == 0) {
        ChunkRenderer::index_count = Chunk::SIZE * Chunk::SIZE * 6;

        std::vector<unsigned int> indices;
        indices.resize(ChunkRenderer::index_count);

        for (int z = 0; z < Chunk::SIZE; z++) {
            for (int x = 0; x < Chunk::SIZE; x++) {
                unsigned int* quad = &indices[(z * Chunk::SIZE + x) * 6];

                unsigned int a = (z + 0) * (Chunk::SIZE + 1) + (x + 0);
                unsigned int b = (z + 1) * (Chunk::SIZE + 1) + (x + 0);
                unsigned int c = (z + 1) * (Chunk::SIZE + 1) + (x + 1);
                unsigned int d = (z + 0) * (Chunk::SIZE + 1) + (x + 1);

                quad[0] = a;
                quad[1] = b;
                quad[2] = d;

                quad[3] = d;
                quad[4] = b;
                quad[5] = c;
            }
        }

        glGenBuffers(1, &ChunkRenderer::EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ChunkRenderer::EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void ChunkRenderer::terminate() {
    if (ChunkRenderer::shader) glDeleteProgram(ChunkRenderer::shader);
    if (ChunkRenderer::VBO_XZ) glDeleteBuffers(1, &ChunkRenderer::VBO_XZ);
    if (ChunkRenderer::EBO) glDeleteBuffers(1, &ChunkRenderer::EBO);
}

GLuint ChunkRenderer::getShader() {
    return ChunkRenderer::shader;
}

ChunkRenderer::ChunkRenderer() {
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO_Y);
}

ChunkRenderer::~ChunkRenderer() {
    if (this->VAO) glDeleteVertexArrays(1, &this->VAO);
    if (this->VBO_Y) glDeleteBuffers(1, &this->VBO_Y);
}

ChunkRenderer::ChunkRenderer(ChunkRenderer&& other) noexcept : VAO(other.VAO), VBO_Y(other.VBO_Y) {
    other.VAO = 0;
    other.VBO_Y = 0;
}

ChunkRenderer& ChunkRenderer::operator=(ChunkRenderer&& other) noexcept {
    if (this != &other) {
        if (this->VAO) glDeleteVertexArrays(1, &this->VAO);
        if (this->VBO_Y) glDeleteBuffers(1, &this->VBO_Y);
        this->VAO = other.VAO;
        this->VBO_Y = other.VBO_Y;
        other.VAO = 0;
        other.VBO_Y = 0;
    }
    return *this;
}

void ChunkRenderer::setHeightBuffer(const float* height_map, size_t bytes) {
    // Upload data
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO_Y);
    glBufferData(GL_ARRAY_BUFFER, bytes, height_map, GL_STATIC_DRAW);

    // Bind VAO
    glBindVertexArray(this->VAO);

    // Bind VBO_XZ
    glBindBuffer(GL_ARRAY_BUFFER, ChunkRenderer::VBO_XZ);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    // Bind VBO_Y
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO_Y);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    // Bind EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ChunkRenderer::EBO);

    glBindVertexArray(0);
}

void ChunkRenderer::draw(const glm::mat4& VP, glm::i64vec2 offset) const {
    glUniform2f(glGetUniformLocation(ChunkRenderer::shader, "uOffset"), static_cast<float>(offset.x), static_cast<float>(offset.y));

    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, ChunkRenderer::index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}