#include "ChunkRenderer.hpp"
#include "rendering/Shader.hpp"
#include <vector>
#include "Chunk.hpp"
#include <stdexcept>

void ChunkRenderer::init() {
    if (ChunkRenderer::shader == 0) {
        ChunkRenderer::shader = compile_shader_program("shaders/chunk.vert", "shaders/chunk.frag");

        glUseProgram(ChunkRenderer::shader);
        glUniform1i(glGetUniformLocation(ChunkRenderer::shader, "uTexture"), 0);
        glUseProgram(0);
    }

    if (ChunkRenderer::SHARED_VBO == 0) {
        std::vector<glm::vec2> xz;
        xz.resize((Chunk::UNITS + 1) * (Chunk::UNITS  + 1));
        for (int z = 0; z <= Chunk::UNITS; z++) {
            for (int x = 0; x <= Chunk::UNITS; x++) {
                int wx = x * Chunk::GRANUALITY;
                int wz = z * Chunk::GRANUALITY;
                xz[z * (Chunk::UNITS + 1) + x] = glm::vec2(wx, wz);
            }
        }

        glGenBuffers(1, &ChunkRenderer::SHARED_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, ChunkRenderer::SHARED_VBO);
        glBufferData(GL_ARRAY_BUFFER, xz.size() * sizeof(glm::vec2), xz.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (ChunkRenderer::GLOBAL_VBO == 0) {
        GLbitfield storageFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_DYNAMIC_STORAGE_BIT;
        GLbitfield mapFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

        glGenBuffers(1, &ChunkRenderer::GLOBAL_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, ChunkRenderer::GLOBAL_VBO);
        glBufferStorage(GL_ARRAY_BUFFER, ChunkRenderer::global_buffer_size, NULL, storageFlags);
        ChunkRenderer::global_buffer = static_cast<float*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, ChunkRenderer::global_buffer_size, mapFlags));
        if (!ChunkRenderer::global_buffer) {
            throw std::runtime_error("ChunkRenderer::global_buffer (static float*) is NULL!\n");
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (ChunkRenderer::EBO == 0) {
        ChunkRenderer::index_count = Chunk::UNITS * Chunk::UNITS * 6;

        std::vector<unsigned int> indices;
        indices.resize(ChunkRenderer::index_count);

        for (int z = 0; z < Chunk::UNITS; z++) {
            for (int x = 0; x < Chunk::UNITS; x++) {
                unsigned int* quad = &indices[(z * Chunk::UNITS + x) * 6];

                unsigned int a = (z + 0) * (Chunk::UNITS + 1) + (x + 0);
                unsigned int b = (z + 1) * (Chunk::UNITS + 1) + (x + 0);
                unsigned int c = (z + 1) * (Chunk::UNITS + 1) + (x + 1);
                unsigned int d = (z + 0) * (Chunk::UNITS + 1) + (x + 1);

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

    if (ChunkRenderer::VAO == 0) {
        glGenVertexArrays(1, &ChunkRenderer::VAO);
        glBindVertexArray(ChunkRenderer::VAO);
        
        glEnableVertexAttribArray(0);
        glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);
        glVertexAttribBinding(0, 0);
        glBindVertexBuffer(0, ChunkRenderer::SHARED_VBO, 0, sizeof(glm::vec2));

        glEnableVertexAttribArray(1);
        glVertexAttribFormat(1, 1, GL_FLOAT, GL_FALSE, 0);
        glVertexAttribBinding(1, 1);

        glBindVertexBuffer(1, 0, 0, sizeof(float));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ChunkRenderer::EBO);

        ChunkRenderer::uOffsetLocation = glGetUniformLocation(ChunkRenderer::shader, "uOffset");

        glBindVertexArray(0);
    }
}

void ChunkRenderer::terminate() {
    if (ChunkRenderer::shader) glDeleteProgram(ChunkRenderer::shader);
    if (ChunkRenderer::VAO) glDeleteBuffers(1, &ChunkRenderer::VAO);
    if (ChunkRenderer::SHARED_VBO) glDeleteBuffers(1, &ChunkRenderer::SHARED_VBO);
    if (ChunkRenderer::GLOBAL_VBO) glDeleteBuffers(1, &ChunkRenderer::GLOBAL_VBO);
    if (ChunkRenderer::EBO) glDeleteBuffers(1, &ChunkRenderer::EBO);
}

GLuint ChunkRenderer::getShader() {
    return ChunkRenderer::shader;
}

GLuint ChunkRenderer::getVAO() {
    return ChunkRenderer::VAO;
}

ChunkRenderer::ChunkRenderer() {
    for (int i = 0; i < ChunkRenderer::GLOBAL_BUFFER_MAX_CHUNKS; i++) {
        if (ChunkRenderer::allocation[i] == false) {
            ChunkRenderer::allocation.set(i);
            this->idx = i;
            break;
        }
    }
    // TODO handle allocation fail
}

ChunkRenderer::~ChunkRenderer() {
    if (this->idx >= 0 && this->idx < ChunkRenderer::GLOBAL_BUFFER_MAX_CHUNKS) ChunkRenderer::allocation.reset(this->idx);
    this->idx = -1;
}

void ChunkRenderer::setHeightBuffer(const float* height_map) {
    if (this->idx < 0 || this->idx >= ChunkRenderer::GLOBAL_BUFFER_MAX_CHUNKS) return;

    size_t floatsPerChunk = ChunkRenderer::CHUNK_BYTES / sizeof(float);
    size_t floatOffset = static_cast<size_t>(this->idx) * floatsPerChunk;

    float* dst = &ChunkRenderer::global_buffer[floatOffset];

    std::copy(height_map, height_map + floatsPerChunk, dst);
}

void ChunkRenderer::draw(const glm::ivec2& coord) const {
    if (this->idx < 0 || this->idx >= ChunkRenderer::GLOBAL_BUFFER_MAX_CHUNKS) return;

    float cx = static_cast<float>(coord.x) * static_cast<float>(Chunk::SIZE);
    float cz = static_cast<float>(coord.y) * static_cast<float>(Chunk::SIZE);
    glUniform2f(ChunkRenderer::uOffsetLocation, cx, cz);

    glBindVertexBuffer(1, ChunkRenderer::GLOBAL_VBO, this->idx * ChunkRenderer::CHUNK_BYTES, sizeof(float));
    glDrawElements(GL_TRIANGLES, ChunkRenderer::index_count, GL_UNSIGNED_INT, 0);
}