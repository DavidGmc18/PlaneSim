#pragma once

#include "TerrainGenerator.hpp"
#include "rendering/TextureCache.hpp"
#include "Chunk.hpp"
#include <vector>
#include "rendering/Material.hpp"

class World {
    std::vector<Chunk> chunks;

    Material material;

public:
    World(TerrainGenerator& generator, TextureCache& cache);

    void render(const glm::mat4& VP) const;
};