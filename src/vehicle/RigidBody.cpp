#include "RigidBody.hpp"
#include "CONSTANTS.h"

void RigidBody::update(float dt) {
    velocity += force / mass * dt;
    velocity += impulse / mass;
    if (gravity) velocity.y -= STANDARD_GRAVITY * dt;

    position += velocity * dt;

    angular_velocity += inverse_inertia * (torque - glm::cross(angular_velocity, inertia * angular_velocity)) * dt;
    angular_velocity += inverse_inertia * angular_impulse;

    orientation += (orientation * glm::quat(0.0f, angular_velocity)) * 0.5f * dt;
    orientation = glm::normalize(orientation);

    force = glm::vec3(0);
    impulse = glm::vec3(0);
    torque = glm::vec3(0);
    angular_impulse = glm::vec3(0);
}

float RigidBody::getMass() const {
    return this->mass;
}

glm::vec3 RigidBody::toWorldDirection(const glm::vec3& direction) const {
    return this->orientation * direction;
}

glm::vec3 RigidBody::toBodyDirection(const glm::vec3& direction) const {
    return glm::inverse(this->orientation) * direction;
}

glm::vec3 RigidBody::toWorldPos(const glm::vec3& point) const {
    return this->position + this->orientation * point;
}

glm::vec3 RigidBody::toBodyPos(const glm::vec3& point) const {
    return glm::inverse(this->orientation) * (point - this->position);
}

glm::vec3 RigidBody::getBodyVelocityAtPoint(const glm::vec3& point) const {
    return toBodyDirection(this->velocity) + glm::cross(this->angular_velocity, point);
}

glm::vec3 RigidBody::getWorldVelocityAtPoint(const glm::vec3& point) const {
    return velocity + glm::cross(toWorldDirection(angular_velocity), toWorldDirection(point));
}

void RigidBody::addBodyForceAtBodyPoint(const glm::vec3& force, const glm::vec3& point) {
    this->force += toWorldDirection(force);
    this->torque += glm::cross(point, force);
}

void RigidBody::addWorldForceAtWorldPoint(const glm::vec3& force, const glm::vec3& point) {
    this->force += force;
    this->torque += glm::cross(toBodyPos(point), toBodyPos(force));
}

void RigidBody::addBodyImpulseAtBodyPoint(const glm::vec3& impulse, const glm::vec3& point) {
    this->impulse += toWorldDirection(impulse);
    this->angular_impulse += glm::cross(point, impulse);
}

void RigidBody::addWorldImpulseAtWorldPoint(const glm::vec3& impulse, const glm::vec3& point) {
    this->impulse += impulse;
    this->angular_impulse += glm::cross(toBodyPos(point), toBodyDirection(impulse));
}