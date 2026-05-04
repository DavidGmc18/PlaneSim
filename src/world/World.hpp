#pragma once

#include "TerrainGenerator.hpp"
#include "Chunk.hpp"
#include "TextureCache.hpp"

class World {
    static constexpr unsigned VIEW_DISTANCE = 1;
    static constexpr unsigned CHUNK_COUNT = (VIEW_DISTANCE * 2 + 1) * (VIEW_DISTANCE * 2 + 1);

    TerrainGenerator generator;
    std::array<Chunk, CHUNK_COUNT> chunks;

public:
    World(TextureCache& cache);

    void draw(GLuint shader) const;
};