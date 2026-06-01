#include "Hitbox.hpp"

Hitbox::Hitbox(glm::vec3 pos, float radius, float k, float d):
    pos(pos), radius(radius), k(k), d(d) {}

glm::vec3 computeClosestPointOnTriangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, const glm::vec3& P) {
    glm::vec3 ab = B - A;
    glm::vec3 ac = C - A;

    // Vertex A
    glm::vec3 ap = P - A;
    float d1 = glm::dot(ab, ap);
    float d2 = glm::dot(ac, ap);
    if (d1 <= 0.0f && d2 <= 0.0f) return A;
    
    // Vertex B
    glm::vec3 bp = P - B;
    float d3 = glm::dot(ab, bp);
    float d4 = glm::dot(ac, bp);
    if (d3 >= 0.0f && d4 <= d3) return B;
    
    // Edge AB
    float vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
        float v = d1 / (d1 - d3);
        return A + v * ab;
    }
    
    // Vertex C
    glm::vec3 cp = P - C;
    float d5 = glm::dot(ab, cp);
    float d6 = glm::dot(ac, cp);
    if (d6 >= 0.0f && d5 <= d6) return C;
    
    // Edge AC
    float vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
        float w = d2 / (d2 - d6);
        return A + w * ac;
    }
    
    // Edge BC
    float va = d3 * d6 - d5 * d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return B + w * (C - B);
    }
    
    // Face
    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom;
    float w = vc * denom;
    return A + v * ab + w * ac;
}

void Hitbox::update(RigidBody* body, World* world, float) {
    // glm::dvec3 P = body->toGlobalPos(this->pos);

    // struct {
    //     glm::dvec3 x;
    //     glm::vec3 normal;
    //     float height = FLT_MAX;
    // } best_hit; 
    // bool hit = false;

    // float radius_sq = radius * radius;

    // const Mesh* mesh = world->getMesh();
    // for (int z = std::floor(P.z - radius); z <= std::ceil(P.z + radius); z++) {
    //     for (int x = std::floor(P.x - radius); x <= std::ceil(P.x + radius); x++) {
    //         std::array<Triangle, 2> triangles = world->getSquare(x, z);
    //         for (Triangle& t : triangles) {
    //             const glm::vec3& A = mesh->vertices[t.v0].pos;
    //             const glm::vec3& B = mesh->vertices[t.v1].pos;
    //             const glm::vec3& C = mesh->vertices[t.v2].pos;

    //             glm::vec3 face_normal = glm::normalize(glm::cross(B - A, C - A));

    //             // TODO computeClosestPointOnTriangle should be in double
    //             glm::dvec3 X = glm::dvec3(computeClosestPointOnTriangle(A, B, C, P));
    //             if (glm::dot(P - X, P - X) > radius_sq) continue;

    //             float height = glm::dot(glm::vec3(P - X), face_normal);
    //             if (height < best_hit.height) {
    //                 best_hit.x = X;
    //                 best_hit.normal = face_normal;
    //                 best_hit.height = height;
    //                 hit = true;
    //             }
    //         }
    //     }
    // }

    // if (hit) {
    //     float compression = std::min((radius - best_hit.height) / radius, 1.0f);
    //     glm::vec3 spring = best_hit.normal * (std::pow(compression, 1.5f) * this->k);
    //     body->addWorldForceAtWorldPoint(spring, best_hit.x);

    //     float normal_vel = glm::dot(body->getGlobalVelocityAtLocal(this->pos), best_hit.normal);
    //     if (normal_vel < 0.0f) {
    //         glm::vec3 damping = best_hit.normal * (-normal_vel * this->d);
    //         body->addWorldImpulseAtWorldPoint(damping, best_hit.x);
    //     }
    // }
}