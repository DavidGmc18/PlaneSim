#pragma once

#include "PhysicPart.hpp"

class Hitbox : public PhysicPart {
    glm::vec3 pos;
    float radius;
    float k;
    float d;

public:
    Hitbox(glm::vec3 pos, float radius, float k, float d);

    void update(RigidBody* body, World* world, float dt) override;
};