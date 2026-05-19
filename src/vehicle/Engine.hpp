#pragma once

#include "PhysicPart.hpp"

class Engine : public PhysicPart {
    glm::vec3 pos;
    glm::vec3 thrust;
    const float* throttle;
    float rpm = 0.0f;
    float spool_speed;
    
public:
    Engine(glm::vec3 pos, glm::vec3 forward, float thrust, float spool_speed, const float* throttle);

    void update(RigidBody* body, World* world, float dt) override;

    float getRPM() const;
};