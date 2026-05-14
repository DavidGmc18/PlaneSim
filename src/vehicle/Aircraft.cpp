#include "Aircraft.hpp"

void Aircraft::update(float dt, World* world) {
    for (Engine& engine : engines) {
        engine.apply_force(this);
    }

    for (Wing& wing : wings) {
        wing.apply_forces(this);
    }

    for (Hitbox& hitbox: hitboxes) {
        hitbox.apply_forces(this, world);
    }

    RigidBody::update(dt);
}

void Aircraft::useCamera(GLuint shader, float aspect) {
    this->camera.update();
    this->camera_distance = std::clamp(this->camera_distance, 0.0f, 100.0f);
    this->camera.pos = this->position - (camera.front * this->camera_distance);
    this->camera.use(shader, aspect);
}

void Aircraft::onMouseMove(float x, float y) {
    this->camera.pitch -= y;
    this->camera.yaw += x;
}

void Aircraft::onMouseScroll(float s) {
    this->camera_distance -= s;
}

const std::vector<Engine>& Aircraft::getEngines() const {
    return engines;
}

const std::vector<Wing>& Aircraft::getWings() const {
    return wings;
}