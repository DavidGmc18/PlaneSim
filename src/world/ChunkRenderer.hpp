#pragma once

#include <glad/glad.h>
#include <stddef.h>
#include <glm/glm.hpp>
#include <bitset>

class ChunkRenderer {
    inline static GLuint shader = 0;

    inline static GLuint VAO = 0;
    inline static GLuint SHARED_VBO = 0;
    inline static GLuint GLOBAL_VBO = 0;
    inline static GLuint EBO = 0;
    inline static GLsizei index_count = 0;

    inline static GLint uOffsetLocation = 0;

    static constexpr int CHUNK_BYTES = (64 +1) * (64 + 1) * sizeof(float);// TODO
    static constexpr int GLOBAL_BUFFER_MAX_CHUNKS = 4096;
    static constexpr int global_buffer_size = GLOBAL_BUFFER_MAX_CHUNKS * CHUNK_BYTES;
    inline static float* global_buffer = NULL;
    inline static std::bitset<GLOBAL_BUFFER_MAX_CHUNKS> allocation = {};

    int idx = -1;

public:
    static void init();
    static void terminate();

    static GLuint getShader();
    static GLuint getVAO();

    ChunkRenderer();
    ~ChunkRenderer();

    void setHeightBuffer(const float* height_map);

    void draw(const glm::ivec2& coord) const;
};