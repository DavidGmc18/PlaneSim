#include "World.hpp"

World::World(int world_size, TerrainGenerator& generator, TextureCache& cache): WORLD_SIZE(world_size) {
    mesh.vertices.resize((WORLD_SIZE + 1) * (WORLD_SIZE + 1));
    mesh.triangles.resize(WORLD_SIZE * WORLD_SIZE * 2);

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
    for (int z = 0; z < WORLD_SIZE; z++) {
        for (int x = 0; x < WORLD_SIZE; x++) {
            Triangle& t0 = mesh.triangles[(z * WORLD_SIZE + x) * 2];
            Triangle& t1 = mesh.triangles[(z * WORLD_SIZE + x) * 2 + 1];

            int rowA = z * (WORLD_SIZE + 1);
            int rowB = rowA + (WORLD_SIZE + 1);

            int a = rowA + x;
            int b = a + 1;
            int c = rowB + x;
            int d = c + 1;

            t0.v0 = a;
            t0.v1 = c;
            t0.v2 = b;

            t1.v0 = b;
            t1.v1 = c;
            t1.v2 = d;
        }
    }

    Vertex::compute_normals(mesh.vertices, mesh.triangles);
    Vertex::compute_tangents(mesh.vertices, mesh.triangles);
    mesh.build();
}

void World::draw(GLuint shader) const {
    glm::mat4 model = glm::mat4(1.0f);
    mesh.render(shader, model);
}

const Mesh* World::getMesh() const {
    return &mesh;
}

std::array<Triangle, 2> World::getSquare(float x, float z) const {
    int row = std::floor(z + (float)WORLD_SIZE/2);
    int col = std::floor(x + (float)WORLD_SIZE/2);

    if (col < 0) col = 0;
    if (row < 0) row = 0;
    if (row >= WORLD_SIZE) row = WORLD_SIZE - 1;
    if (col >= WORLD_SIZE) col = WORLD_SIZE - 1;

    int rowA = row * (WORLD_SIZE + 1);
    int rowB = rowA + WORLD_SIZE + 1;

    int a = rowA + col;
    int b = a + 1;
    int c = rowB + col;
    int d = c + 1;

    std::array<Triangle, 2> triangles;

    triangles[0].v0 = a;
    triangles[0].v1 = c;
    triangles[0].v2 = b;

    triangles[1].v0 = b;
    triangles[1].v1 = c;
    triangles[1].v2 = d;

    return triangles;
}