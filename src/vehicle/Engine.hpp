#pragma once

#include "RigidBody.hpp"
#include <glm/glm.hpp>

struct Engine {
    glm::vec3 pos;
    glm::vec3 thrust;
    float throttle = 0.0f;

    Engine(glm::vec3 pos, glm::vec3 forward, float thrust, float throttle = 0.0f);

    void apply_force(RigidBody* rigid_body);

    void addThrottle(float value);
};