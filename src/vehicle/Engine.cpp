#include "Engine.hpp"

Engine::Engine(glm::vec3 pos, glm::vec3 forward, float thrust, float throttle)
    : pos(pos), thrust(forward * thrust), throttle(glm::clamp(throttle, 0.0f, 1.0f)) {}

void Engine::apply_force(RigidBody* rigid_body) {
    this->throttle = glm::clamp(this->throttle, 0.0f, 1.0f);
    glm::vec3 force = throttle * thrust;
    rigid_body->addBodyForceAtBodyPoint(force, pos);
}

void Engine::addThrottle(float value) {
    this->throttle = glm::clamp(this->throttle + value, 0.0f, 1.0f);
}