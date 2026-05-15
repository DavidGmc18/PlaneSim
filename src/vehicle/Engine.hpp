#pragma once

#include "RigidBody.hpp"
#include <glm/glm.hpp>

class Engine {
    glm::vec3 pos;
    glm::vec3 thrust;
    float throttle = 0.0f;
    float rpm = 0.0f;
    
public:
    Engine(glm::vec3 pos, glm::vec3 forward, float thrust, float throttle = 0.0f);

    void update(float dt);

    void apply_force(RigidBody* rigid_body);

    void addThrottle(float value);
    float getThrottle() const;
    float getRPM() const;
};