#pragma once

#include <glm/glm.hpp>
#include "TerrainGenerator.hpp"
#include "ChunkRenderer.hpp"
#include <vector>

#include "rendering/Material.hpp"

class Chunk {
public:
    static constexpr int SIZE = 16;
private:
    glm::ivec2 coord;

    std::vector<float> height_map;
    ChunkRenderer renderer;

public:
    Chunk(glm::ivec2 coord, const TerrainGenerator& generator);

    void draw(const glm::mat4& VP) const;

    glm::i64vec2 getOffset() const;
};