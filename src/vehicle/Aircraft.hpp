#pragma once

#include "RigidBody.hpp"
#include "Wing.hpp"
#include "Engine.hpp"
#include "Hitbox.hpp"

#include <vector>

class Aircraft : public RigidBody {
protected:
    std::vector<Engine> engines; 
    std::vector<Wing> wings; 
    std::vector<Hitbox> hitboxes;

public:
    virtual void update(float dt, World* world);
};