#pragma once

#include "TerrainGenerator.hpp"
#include "Chunk.hpp"
#include "TextureCache.hpp"

class World {
    static constexpr int VIEW_DISTANCE = 2;
    static constexpr int CHUNK_COUNT = 4 * VIEW_DISTANCE * VIEW_DISTANCE;

    TerrainGenerator generator;
    std::array<Chunk, CHUNK_COUNT> chunks;

public:
    World(TextureCache& cache);

    void draw(GLuint shader) const;
};