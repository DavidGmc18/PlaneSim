#pragma once

#include "RigidBody.hpp"
#include <glm/glm.hpp>
#include <world/World.hpp>

class Hitbox {
    glm::vec3 pos;
    float radius;
    float k;
    float d;

public:
    Hitbox(glm::vec3 pos, float radius, float k, float d);

    void apply_forces(RigidBody* rigid_body, World* world);
};