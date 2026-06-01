#pragma once

#include <glm/glm.hpp>
#include "TerrainGenerator.hpp"
#include "ChunkRenderer.hpp"
#include <vector>

#include "rendering/Material.hpp"

class Chunk {
public:
    static constexpr int SIZE = 64;
    static constexpr int GRANUALITY = 1;
    static constexpr int UNITS = SIZE / GRANUALITY; // TODO assert divisible
private:
    bool dirty = true;

    glm::ivec2 coord;

    std::vector<float> height_map;
    ChunkRenderer renderer;

public:
    Chunk();

    void load(glm::ivec2 coord, const TerrainGenerator& generator);
    void unload();

    void draw() const;
};