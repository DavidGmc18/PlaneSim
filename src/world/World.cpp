#include "World.hpp"

World::World(TextureCache& cache) {
    generator = TerrainGenerator(0.0f, 20.0f, 1.0f);

    for (Chunk& chunk : chunks) {
        chunk = Chunk(cache);
    }

    for (int z = -VIEW_DISTANCE; z < VIEW_DISTANCE; z++) {
        int row_offset= (z + VIEW_DISTANCE) * VIEW_DISTANCE * 2;
        for (int x = -VIEW_DISTANCE; x < VIEW_DISTANCE; x++) {
            int collumn_offset = x + VIEW_DISTANCE;
            chunks[row_offset+collumn_offset].load(x, z, generator);
        }
    }
}

void World::draw(GLuint shader) const {
    for (const Chunk& chunk : chunks) {
        chunk.draw(shader);
    }
}