#pragma once

#include "Mesh.hpp"
#include "TerrainGenerator.hpp"
#include "TextureCache.hpp"

class Chunk {
public:
    static constexpr unsigned CHUNK_SIZE = 64;

    enum State {
        CHUNK_UNLOADED,
        CHUNK_DIRTY,
        CHUNK_LOADED
    };

private:
    State state = CHUNK_UNLOADED;
    int chunk_x, chunk_z;
    Mesh mesh;
    

public:
    Chunk() = default;
    Chunk(TextureCache& cache);

    void load(int chunk_x, int chunk_z, TerrainGenerator& generator);

    void draw(GLuint shader) const;
}; 