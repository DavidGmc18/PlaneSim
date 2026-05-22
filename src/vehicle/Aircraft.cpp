#include "Aircraft.hpp"

Aircraft::~Aircraft() {
    for (PhysicPart* part : parts) {
        delete part;
    }
}

void Aircraft::update(World* world, float dt) {
    RigidBody::update(world, dt);
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