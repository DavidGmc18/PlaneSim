#include "Chunk.hpp"
#include <glm/ext/matrix_transform.hpp>

Chunk::Chunk(TextureCache& cache) {
    mesh.vertices.resize((CHUNK_SIZE + 1) * (CHUNK_SIZE + 1));
    mesh.indices.resize(CHUNK_SIZE * CHUNK_SIZE * 6);

    GLuint grass_diffuse = cache.get("assets/grass/diffuse.jpg");
    GLuint grass_specular = cache.get("assets/grass/specular.jpg");
    GLuint grass_normal = cache.get("assets/grass/normal.jpg");
    GLuint grass_shininess = cache.get("assets/grass/shininess.jpg");

    mesh.material = Material(grass_diffuse, grass_specular, grass_normal, grass_shininess, 1.0f);

    // Vertices
    for (unsigned z = 0; z < (CHUNK_SIZE + 1); z++) {
        for (unsigned x = 0; x < (CHUNK_SIZE + 1); x++) {
            Vertex& vertex = mesh.vertices[z * (CHUNK_SIZE + 1) + x];
            vertex.pos = glm::vec3(x, 0.0f, z);
            vertex.uv = glm::vec2(x, z);
        }
    }

    // Indices
    for (unsigned z = 0; z < CHUNK_SIZE; z++) {
        for (unsigned x = 0; x < CHUNK_SIZE; x++) {
            unsigned int* ptr = &mesh.indices[(z * CHUNK_SIZE + x) * 6];

            int rowA = z * (CHUNK_SIZE + 1);
            int rowB = rowA + (CHUNK_SIZE + 1);

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

    state = CHUNK_DIRTY;
}

void Chunk::load(int chunk_x, int chunk_z, TerrainGenerator& generator) {
    if (state == CHUNK_UNLOADED) return;

    this->chunk_x = chunk_x;
    this->chunk_z = chunk_z;

    for (unsigned z = 0; z < (CHUNK_SIZE + 1); z++) {
        for (unsigned x = 0; x < (CHUNK_SIZE + 1); x++) {
            mesh.vertices[z * (CHUNK_SIZE + 1) + x].pos.y = generator.getHeight((float)chunk_x * CHUNK_SIZE + x, (float)chunk_z * CHUNK_SIZE + z);
        }
    }

    compute_normals(mesh.vertices, mesh.indices);
    compute_tangents(mesh.vertices, mesh.indices);
    mesh.build();

    state = CHUNK_LOADED;
}

void Chunk::draw(GLuint shader) const {
    if (state != CHUNK_LOADED) return;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3((float)chunk_x * CHUNK_SIZE, 0.0f, (float)chunk_z * CHUNK_SIZE));
    mesh.draw(shader, model);
}