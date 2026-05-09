#pragma once

#include "TerrainGenerator.hpp"
#include "TextureCache.hpp"
#include "Mesh.hpp"

class World {
    static constexpr int WORLD_SIZE = 256;
    Mesh mesh;

public:
    World(TerrainGenerator& generator, TextureCache& cache);

    void draw(GLuint shader) const;

    const Mesh* getMesh() const;
};