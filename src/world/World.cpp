#include "World.hpp"

World::World(TerrainGenerator& generator, TextureCache& cache) {
    mesh.vertices.resize((WORLD_SIZE + 1) * (WORLD_SIZE + 1));
    mesh.indices.resize(WORLD_SIZE * WORLD_SIZE * 6);

    GLuint grass_diffuse = cache.get("assets/grass/diffuse.jpg");
    GLuint grass_specular = cache.get("assets/grass/specular.jpg");
    GLuint grass_normal = cache.get("assets/grass/normal.jpg");
    GLuint grass_shininess = cache.get("assets/grass/shininess.jpg");

    mesh.material = Material(grass_diffuse, grass_specular, grass_normal, grass_shininess, 1.0f);

    // Vertices
    for (int i = 0; i < (WORLD_SIZE + 1); i++) {
        for (int j = 0; j < (WORLD_SIZE + 1); j++) {
            Vertex& vertex = mesh.vertices[i * (WORLD_SIZE + 1) + j];
            float x = (float)j - ((float)WORLD_SIZE/2);
            float z = (float)i - ((float)WORLD_SIZE/2);
            float y = generator.getHeight(x, z);
            vertex.pos = glm::vec3(x, y, z);
            vertex.uv = glm::vec2(x, z);
        }
    }

    // Indices
    for (unsigned z = 0; z < WORLD_SIZE; z++) {
        for (unsigned x = 0; x < WORLD_SIZE; x++) {
            unsigned int* ptr = &mesh.indices[(z * WORLD_SIZE + x) * 6];

            int rowA = z * (WORLD_SIZE + 1);
            int rowB = rowA + (WORLD_SIZE + 1);

            int a = rowA + x;
            int b = a + 1;
            int c = rowB + x;
            int d = c + 1;

            ptr[0] = a;
            ptr[1] = c;
            ptr[2] = b;

            ptr[3] = b;
            ptr[4] = c;
            ptr[5] = d;
        }
    }

    Vertex::compute_normals(mesh.vertices, mesh.indices);
    Vertex::compute_tangents(mesh.vertices, mesh.indices);
    mesh.build();
}

void World::draw(GLuint shader) const {
    glm::mat4 model = glm::mat4(1.0f);
    mesh.draw(shader, model);
}