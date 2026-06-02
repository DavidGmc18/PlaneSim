#include "Chunk.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <vector>
#include <rendering/Vertex.hpp>

Chunk::Chunk() {
    this->height_map.resize((Chunk::UNITS + 1) * (Chunk::UNITS  + 1));
}

void Chunk::load(glm::ivec2 coord, const TerrainGenerator& generator) {
    this->coord = coord;

    int cx = this->coord.x * Chunk::SIZE;
    int cz = this->coord.y * Chunk::SIZE;
    for (int z = 0; z <= Chunk::UNITS; z++) {
        for (int x = 0; x <= Chunk::UNITS; x++) {
            int wx = cx + x * Chunk::GRANUALITY;
            int wz = cz + z * Chunk::GRANUALITY;
            this->height_map[z * (Chunk::UNITS + 1) + x] = generator.getHeight(wx, wz);
        }
    }

    this->renderer.setHeightBuffer(this->height_map.data());
    this->dirty = false;
}

void Chunk::unload() {
    this->dirty = true;
}

void Chunk::draw() const {
    if (this->dirty) return;
    this->renderer.draw(this->coord);
}






// glm::vec2 Chunk::VBO_XZ[CHUNK_VERTICES];
// unsigned int Chunk::EBO[CHUNK_INDICES];

// void Chunk::init() {
    // for (size_t z = 0; z < CHUNK_ROW; z++) {
    //     for (size_t x = 0; x < CHUNK_ROW; x++) {
    //         Chunk::VBO_XZ[z * CHUNK_ROW + x] = glm::vec2(x, z);
    //     }
    // }

    // for (size_t z = 0; z < CHUNK_SIZE; z++) {
    //     for (size_t x = 0; x < CHUNK_SIZE; x++) {
    //         unsigned int* square = &Chunk::EBO[(z * CHUNK_SIZE + x) * 6];

    //         unsigned int a = (z + 0) * CHUNK_ROW + (x + 0);
    //         unsigned int b = (z + 1) * CHUNK_ROW + (x + 0);
    //         unsigned int c = (z + 1) * CHUNK_ROW + (x + 1);
    //         unsigned int d = (z + 0) * CHUNK_ROW + (x + 1);

    //         square[0] = a;
    //         square[1] = b;
    //         square[2] = d;

    //         square[3] = d;
    //         square[4] = b;
    //         square[5] = c;
    //     }
    // }
// }

// Chunk::Chunk(const TerrainGenerator* generator, glm::i64vec2 chunk_pos): chunk_pos(chunk_pos) {
//     for (size_t z = 0; z < CHUNK_ROW; z++) {
//         for (size_t x = 0; x < CHUNK_ROW; x++) {
//             int64_t wx = this->chunk_pos.x * CHUNK_SIZE + x;
//             int64_t wz = this->chunk_pos.y * CHUNK_SIZE + z;
//             this->height_map[z * CHUNK_ROW + x] = generator->getHeight(wx, wz);
//         }
//     }

//     auto getHeight = [this, generator](int64_t x, int64_t z) {
//         if (x >= 0 && x <= CHUNK_SIZE && z >= 0 && z <= CHUNK_SIZE) {
//             return this->height_map[z * CHUNK_ROW + x];
//         }
//         int64_t wx = this->chunk_pos.x * CHUNK_SIZE + x;
//         int64_t wz = this->chunk_pos.y * CHUNK_SIZE + z;
//         return generator->getHeight(wx, wz);
//     };

//     glm::vec3 normals[(CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * 2]; // TODO this is stack, maybe use std::vector

//     auto getQuadNormals = [&normals](int64_t x, int64_t z) {
//         return &normals[((z + 1) * (CHUNK_SIZE + 2) + (x + 1)) * 2];
//     };
    
//     for (int64_t z = -1; z < (CHUNK_SIZE + 1); z++) {
//         for (int64_t x = -1; x < (CHUNK_SIZE + 1); x++) {
//             glm::vec3 A = glm::vec3((x+0), getHeight((x+0), (z+0)), (z+0));
//             glm::vec3 B = glm::vec3((x+0), getHeight((x+0), (z+1)), (z+1));
//             glm::vec3 C = glm::vec3((x+1), getHeight((x+1), (z+1)), (z+1));
//             glm::vec3 D = glm::vec3((x+1), getHeight((x+1), (z+0)), (z+0));

//             glm::vec3* quad_normals = getQuadNormals(x, z);
//             quad_normals[0] = glm::normalize(glm::cross(B - A, D - A)); 
//             quad_normals[1] = glm::normalize(glm::cross(B - D, C - D)); 
//         }
//     }

//     for (int64_t z = 0; z < CHUNK_ROW; z++) {
//         for (int64_t x = 0; x < CHUNK_ROW; x++) {
//             glm::vec3& N = this->normal_map[z * CHUNK_ROW + x];

//             glm::vec3* quad_normals = getQuadNormals(x, z-1);
//             N += quad_normals[-1];
//             N += quad_normals[ 0];
//             N += quad_normals[ 1];

//             quad_normals = getQuadNormals(x, z);
//             N += quad_normals[-2];
//             N += quad_normals[-1];
//             N += quad_normals[ 0];

//             N = glm::normalize(N);
//         }
//     }
// }