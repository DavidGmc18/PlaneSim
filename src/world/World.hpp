#pragma once

#include "TerrainGenerator.hpp"
#include "TextureCache.hpp"
#include "Mesh.hpp"

class World {
    const int WORLD_SIZE;
    Mesh mesh;

public:
    World(int world_size, TerrainGenerator& generator, TextureCache& cache);

    void draw(GLuint shader) const;

    const Mesh* getMesh() const;

    std::array<Triangle, 2> getSquare(float x, float z) const;
};