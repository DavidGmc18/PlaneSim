#include "RigidBody.hpp"
#include "CONSTANTS.h"

void RigidBody::update(float dt) {
    glm::vec3 acceleration = force / mass;
    if (gravity) acceleration.y -= STANDARD_GRAVITY;

    velocity += acceleration * dt;
    position += velocity * dt;

    angular_velocity += inverse_inertia * (torque - glm::cross(angular_velocity, inertia * angular_velocity)) * dt;
    orientation += (orientation * glm::quat(0.0f, angular_velocity)) * (0.5f * dt);
    orientation = glm::normalize(orientation);

    force = glm::vec3(0);
    torque = glm::vec3(0);
}

// Body-space to world-space 
glm::vec3 RigidBody::transformDirection(const glm::vec3& direction) const {
    return orientation * direction;
}

// World-space to body-space
glm::vec3 RigidBody::inverseTransformDirection(const glm::vec3& direction) const {
    return glm::inverse(orientation) * direction;
}

// Get body-space velocity for body-space point
glm::vec3 RigidBody::getVelocityAtPoint(const glm::vec3& point) const {
    return inverseTransformDirection(velocity) + glm::cross(angular_velocity, point);
}

// Apply force from body-space point
void RigidBody::addForceAtPoint(const glm::vec3& force, const glm::vec3& point) {
    this->force += transformDirection(force);
    this->torque += glm::cross(point, force);
}