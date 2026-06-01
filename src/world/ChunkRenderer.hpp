#pragma once

#include <glad/glad.h>
#include <stddef.h>
#include <glm/glm.hpp>

class ChunkRenderer {
    inline static GLuint shader = 0;
    inline static GLuint VBO_XZ = 0;
    inline static GLuint EBO = 0;
    inline static GLsizei index_count = 0;

    GLuint VAO = 0;
    GLuint VBO_Y = 0;

public:
    static void init();
    static void terminate();

    static GLuint getShader();

    ChunkRenderer();
    ~ChunkRenderer();

    ChunkRenderer(ChunkRenderer&& other) noexcept;
    ChunkRenderer& operator=(ChunkRenderer&& other) noexcept;
    ChunkRenderer(const ChunkRenderer&) = delete;
    ChunkRenderer& operator=(const ChunkRenderer&) = delete;

    void setHeightBuffer(const float* height_map, size_t bytes);

    void draw(const glm::mat4& VP, glm::i64vec2 offset) const;
};