#include "Engine.hpp"

Engine::Engine(glm::vec3 pos, glm::vec3 forward, float thrust, const float* throttle)
    : pos(pos), thrust(forward * thrust), throttle(throttle) {}

void Engine::update(RigidBody* body, World* world, float dt) {
    if (!this->throttle) return;

    float delta = glm::clamp(*this->throttle, 0.0f, 1.0f) - this->rpm;
    delta = (delta > 0.0f ? 1.0f : -1.0f) * std::pow(std::abs(delta), 0.5);
    if (std::isnormal(delta)) {
        this->rpm += delta * 0.1f * dt;
    }

    glm::vec3 force = std::pow(this->rpm, 2.0f) * thrust;
    body->addBodyForceAtBodyPoint(force, pos);
}

float Engine::getRPM() const {
    return this->rpm;
}