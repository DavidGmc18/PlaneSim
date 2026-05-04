#include "World.hpp"

World::World(TextureCache& cache) {
    generator = TerrainGenerator(0.0f, 20.0f, 1.0f);

    for (Chunk& chunk : chunks) {
        chunk = Chunk(cache);
    }

    chunks[0].load( 0,  0, generator);
    chunks[1].load(-1,  0, generator);
    chunks[2].load( 0, -1, generator);
    chunks[3].load(-1, -1, generator);
}

void World::draw(GLuint shader) const {
    for (const Chunk& chunk : chunks) {
        chunk.draw(shader);
    }
}