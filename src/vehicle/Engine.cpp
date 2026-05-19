#include "Engine.hpp"

Engine::Engine(glm::vec3 pos, glm::vec3 forward, float thrust, float spool_speed, const float* throttle)
    : pos(pos), thrust(forward * thrust), throttle(throttle), spool_speed(spool_speed) {}

void Engine::update(RigidBody* body, World* world, float dt) {
    if (!this->throttle) return;

    float spool_speed = 2.0f;
    this->rpm = glm::mix(this->rpm, *this->throttle, this->spool_speed * dt);
    this->rpm = glm::clamp(this->rpm, 0.0f, 1.0f);
    if (this->rpm < 0.0f) this->rpm = 0.0f;

    glm::vec3 force = std::pow(this->rpm, 1.5f) * thrust;
    body->addBodyForceAtBodyPoint(force, pos);
}

float Engine::getRPM() const {
    return this->rpm;
}