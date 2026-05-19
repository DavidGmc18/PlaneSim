#include "Aircraft.hpp"

AircraftControl::AircraftControl(float min, float max): min(min), max(max) {
    this->value = glm::clamp(this->value, this->min, this->max);
}

void AircraftControl::setAbsolute(float value) {
    this->value = value;
    this->relative = 0.0f;
}

void AircraftControl::setRelative(float relative) {
    this->relative = relative;
}

void AircraftControl::update(float dt) {
    if (!this->relative) return;
    this->value = glm::clamp(this->value + dt * this->relative, this->min, this->max);
}

const float* AircraftControl::get() const {
    return &value;
}


Aircraft::~Aircraft() {
    for (PhysicPart* part : parts) {
        delete part;
    }
}

void Aircraft::update(float dt, World* world) {
    for (AircraftControl& control : controls) {
        control.update(dt);
    }

    for (PhysicPart* part : parts) {
        part->update(this, world, dt);
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

void Aircraft::onJoyMotion(float value, AircraftControls control, bool relative) {
    if (control < 0 || control >= AIRCRAFT_CONTROLS_COUNT) return;

    if (relative) {
        this->controls[control].setRelative(value);
    } else {
        this->controls[control].setAbsolute(value);
    }
}

std::span<const PhysicPart* const> Aircraft::getPhysicParts() const {
    return this->parts;
}

std::span<const AircraftControl> Aircraft::getControls() const {
    return this->controls;
}