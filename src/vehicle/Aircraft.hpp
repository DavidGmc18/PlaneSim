#pragma once

#include "RigidBody.hpp"
#include "Wing.hpp"
#include <vector>

class Aircraft : public RigidBody {
protected:
    std::vector<Wing> wings; 
    // std::vector<Engine> engines; 

public:
    void update(float dt) override {
        // for (Wing& wing : wings) {
        //     wing.apply_force(this);
        // }

        // for (Engine& engine : engines) {
        //     engine.apply_force(this);
        // }

        RigidBody::update(dt);
    }
};