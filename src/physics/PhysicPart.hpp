#pragma once

#include "RigidBody.hpp"
#include "world/World.hpp"

struct PhysicPart {
    virtual ~PhysicPart() = default;
    virtual void update(RigidBody* body, World* world, float dt) = 0;
};