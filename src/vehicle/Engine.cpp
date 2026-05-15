#include "Engine.hpp"

Engine::Engine(glm::vec3 pos, glm::vec3 forward, float thrust, float throttle)
    : pos(pos), thrust(forward * thrust), throttle(glm::clamp(throttle, 0.0f, 1.0f)) {}

void Engine::update(float dt) {
    float delta = this->throttle - this->rpm;
    delta = (delta > 0.0f ? 1.0f : -1.0f) * std::pow(std::abs(delta), 0.5);
    this->rpm += delta * 0.1f * dt;
}

void Engine::apply_force(RigidBody* rigid_body) {
    glm::vec3 force = std::pow(this->rpm, 2.0f) * thrust;
    rigid_body->addBodyForceAtBodyPoint(force, pos);
}

void Engine::addThrottle(float value) {
    this->throttle = glm::clamp(this->throttle + value, 0.0f, 1.0f);
}

float Engine::getThrottle() const {
    return this->throttle;
}

float Engine::getRPM() const {
    return this->rpm;
}