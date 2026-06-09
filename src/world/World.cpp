#include "World.hpp"

World::World(int world_size, TerrainGenerator& generator, TextureCache& cache): WORLD_SIZE(world_size) {
    mesh.vertices.resize((UNIT_COUNT + 1) * (UNIT_COUNT + 1));
    mesh.triangles.resize(UNIT_COUNT * UNIT_COUNT * 2);

    GLuint grass_diffuse = cache.get("assets/grass/diffuse.jpg");
    GLuint grass_specular = cache.getColor(glm::vec3(0.1f)); //cache.get("assets/grass/specular.jpg");
    GLuint grass_normal = cache.get("assets/grass/normal.jpg");
    GLuint grass_shininess = cache.get("assets/grass/shininess.jpg");

    mesh.material = Material(grass_diffuse, grass_specular, grass_normal, grass_shininess, 1.0f);

    // Vertices
    for (int i = 0; i < (UNIT_COUNT + 1); i++) {
        for (int j = 0; j < (UNIT_COUNT + 1); j++) {
            Vertex& vertex = mesh.vertices[i * (UNIT_COUNT + 1) + j];
            float x = (float)(World::UNIT_SIZE * j) - ((float)WORLD_SIZE/2);
            float z = (float)(World::UNIT_SIZE * i) - ((float)WORLD_SIZE/2);
            float y = generator.getHeight(x, z);
            vertex.pos = glm::vec3(x, y, z);
            vertex.uv = glm::vec2(x, z);
        }
    }

    // Indices
    for (int z = 0; z < UNIT_COUNT; z++) {
        for (int x = 0; x < UNIT_COUNT; x++) {
            Triangle& t0 = mesh.triangles[(z * UNIT_COUNT + x) * 2];
            Triangle& t1 = mesh.triangles[(z * UNIT_COUNT + x) * 2 + 1];

            int rowA = z * (UNIT_COUNT + 1);
            int rowB = rowA + (UNIT_COUNT + 1);

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

void World::draw(GLuint shader, const glm::dmat4& view, const glm::mat4& projection) const {
    glm::dmat4 model = glm::dmat4(1.0f);
    mesh.render(shader, model, view, projection);
}

const Mesh* World::getMesh() const {
    return &mesh;
}

std::array<Triangle, 2> World::getSquare(float x, float z) const {
    int row = std::floor((z / (float)World::UNIT_SIZE) + (float)UNIT_COUNT/2);
    int col = std::floor((x / (float)World::UNIT_SIZE) + (float)UNIT_COUNT/2);

    if (col < 0) col = 0;
    if (row < 0) row = 0;
    if (row >= UNIT_COUNT) row = UNIT_COUNT - 1;
    if (col >= UNIT_COUNT) col = UNIT_COUNT - 1;

    int rowA = row * (UNIT_COUNT + 1);
    int rowB = rowA + UNIT_COUNT + 1;

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