#include "World.hpp"
#include <glm/gtc/type_ptr.hpp>

World::World(TerrainGenerator& generator, TextureCache& cache) {
    GLuint grass_diffuse = cache.get("assets/grass/diffuse.jpg");
    GLuint grass_specular = cache.get("assets/grass/specular.jpg");
    GLuint grass_normal = cache.get("assets/grass/normal.jpg");
    GLuint grass_shininess = cache.get("assets/grass/shininess.jpg");

    this->material = Material(grass_diffuse, grass_specular, grass_normal, grass_shininess, 1.0f);

    constexpr int N = 32;
    this->chunks.resize(N * N * 4);
    for (int z = -N; z < N; z++) {
        for (int x = -N; x < N; x++) {
            this->chunks.emplace_back();
            glm::ivec2 coord = glm::ivec2(x, z);
            this->chunks.back().load(coord, generator);
        }
    }
}

void World::render(const glm::mat4& VP) const {
    glUseProgram(ChunkRenderer::getShader());
    glBindVertexArray(ChunkRenderer::getVAO());

    glUniformMatrix4fv(glGetUniformLocation(ChunkRenderer::getShader(), "uVP"), 1, GL_FALSE, glm::value_ptr(VP));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material.diffuse);

    for (const Chunk& chunk : this->chunks) {
        chunk.draw();
    }

    glBindVertexArray(0);
}