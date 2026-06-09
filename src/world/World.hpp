#pragma once

#include "TerrainGenerator.hpp"
#include "rendering/TextureCache.hpp"
#include "rendering/Mesh.hpp"

class World {
    const int WORLD_SIZE;
    Mesh mesh;

    static constexpr int UNIT_SIZE = 16;
    const int UNIT_COUNT = WORLD_SIZE / UNIT_SIZE;

public:
    World(int world_size, TerrainGenerator& generator, TextureCache& cache);

    void draw(GLuint shader, const glm::dmat4& view, const glm::mat4& projection) const;

    const Mesh* getMesh() const;

    std::array<Triangle, 2> getSquare(float x, float z) const;
};