#pragma once

#include "RigidBody.hpp"
#include "world/World.hpp"

struct PhysicPart {
    virtual void update(RigidBody* body, World* world, float dt) = 0;
};