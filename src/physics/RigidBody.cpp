#include "RigidBody.hpp"
#include "CONSTANTS.h"
#include "PhysicPart.hpp"
#include "Joint.hpp"

RigidBody::~RigidBody() {
    for (PhysicPart* part : this->parts) {
        delete part;
    }

    for (Joint* joint : this->joints) {
        delete joint;
    }
}

void RigidBody::computeVelocity(float dt) {;
    this->velocity += this->force / this->mass * dt;
    this->velocity += this->impulse / this->mass;

    this->force = glm::vec3(0);
    this->impulse = glm::vec3(0);
}

void RigidBody::computeAngularVelocity(float dt) {;
    this->angular_velocity += this->inverse_inertia * this->torque * dt;
    this->angular_velocity += this->inverse_inertia * this->angular_impulse;
    
    this->torque = glm::vec3(0);
    this->angular_impulse = glm::vec3(0);
}

glm::dvec3 RigidBody::getPredictedPosition(float dt) const {
    return this->position + glm::dvec3(this->velocity * dt);
}

glm::quat RigidBody::getPredictedOrientation(float dt) const {
    return glm::normalize(this->orientation + ((this->orientation * glm::quat(0.0f, this->angular_velocity)) * 0.5f * dt));
}

void RigidBody::update(World* world, float dt) {
    if (this->gravity) this->force.y -= STANDARD_GRAVITY * this->mass;

    for (PhysicPart* part : this->parts) {
        part->update(this, world, dt);
    }
}

void RigidBody::solve(float dt) {
    this->computeVelocity(dt);
    this->computeAngularVelocity(dt);

    for (int i = 0; i < JOINT_ITERATIONS; i++) {
        for (Joint* joint : this->joints) {
            if (joint) joint->solve(this, dt);
        }
        
        this->computeVelocity(dt);
        this->computeAngularVelocity(dt);
        for (Joint* joint : this->joints) {
            if (joint && joint->getChild()) {
                joint->getChild()->computeVelocity(dt);
                joint->getChild()->computeAngularVelocity(dt);
            }
        }
    }
}

void RigidBody::apply(float dt) {
    glm::vec3 gyro_torque = -glm::cross(this->angular_velocity, this->inertia * this->angular_velocity);
    this->angular_velocity += this->inverse_inertia * gyro_torque * dt;

    this->position = this->getPredictedPosition(dt);
    this->orientation = this->getPredictedOrientation(dt);
}


glm::dvec3 RigidBody::getPosition() const {
    return this->position;
}

glm::quat RigidBody::getOrientation() const {
    return this->orientation;
}

glm::vec3 RigidBody::getVelocity() const {
    return this->velocity;
}

glm::vec3 RigidBody::getAngularVelocity() const {
    return this->angular_velocity;
}

float RigidBody::getMass() const {
    return this->mass;
}

std::span<PhysicPart* const> RigidBody::getPhysicParts() const {
    return this->parts;
}

std::span<Joint* const> RigidBody::getJoints() const {
    return this->joints;
}

glm::vec3 RigidBody::toWorldDirection(const glm::vec3& direction) const {
    return this->orientation * direction;
}

glm::vec3 RigidBody::toBodyDirection(const glm::vec3& direction) const {
    return glm::conjugate(this->orientation) * direction;
}

glm::dvec3 RigidBody::toWorldPos(const glm::vec3& point) const {
    return this->position + glm::dvec3(this->orientation * point);
}

glm::vec3 RigidBody::toBodyPos(const glm::dvec3& point) const {
    return glm::conjugate(this->orientation) * glm::vec3(point - this->position);
}

glm::vec3 RigidBody::getBodyVelocityAtPoint(const glm::vec3& point) const {
    return toBodyDirection(this->velocity) + glm::cross(this->angular_velocity, point);
}

glm::vec3 RigidBody::getWorldVelocityAtPoint(const glm::vec3& point) const {
    return this->velocity + glm::cross(toWorldDirection(angular_velocity), toWorldDirection(point));
}

glm::mat3 RigidBody::getWorldInverseInertia() const {
    glm::mat3 rotation = glm::mat3_cast(this->orientation);
    return rotation * this->inverse_inertia * glm::transpose(rotation);
}

void RigidBody::addBodyForceAtBodyPoint(const glm::vec3& force, const glm::vec3& point) {
    this->force += toWorldDirection(force);
    this->torque += glm::cross(point, force);
}

void RigidBody::addWorldForceAtWorldPoint(const glm::vec3& force, const glm::dvec3& point) {
    this->force += force;
    this->torque += glm::cross(toBodyPos(point), toBodyDirection(force));
}

void RigidBody::addBodyImpulseAtBodyPoint(const glm::vec3& impulse, const glm::vec3& point) {
    this->impulse += toWorldDirection(impulse);
    this->angular_impulse += glm::cross(point, impulse);
}

void RigidBody::addWorldImpulseAtWorldPoint(const glm::vec3& impulse, const glm::dvec3& point) {
    this->impulse += impulse;
    this->angular_impulse += glm::cross(toBodyPos(point), toBodyDirection(impulse));
}

void RigidBody::addTorque(const glm::vec3& torque) {
    this->torque += torque;
}

void RigidBody::addBodyAngularImpulse(const glm::vec3& angular_impulse) {
    this->angular_impulse += angular_impulse;
}

void RigidBody::addWorldAngularImpulse(const glm::vec3& angular_impulse) {
    this->angular_impulse += toBodyDirection(angular_impulse);
}