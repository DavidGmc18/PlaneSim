#include "RigidBody.hpp"
#include "Physics.hpp"
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

void RigidBody::computeVelocity(float dt) {
    glm::vec3 delta_vel = (this->force / this->mass * dt) + (this->impulse / this->mass);
    this->velocity += delta_vel;

    this->force = glm::vec3(0);
    this->impulse = glm::vec3(0);

    this->acceleration += delta_vel / dt;
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
    this->acceleration = glm::vec3(0);

    if (this->gravity) {
        this->force.y -= phy::STANDARD_GRAVITY * this->mass;
        this->acceleration += glm::vec3(0.0f, phy::STANDARD_GRAVITY, 0.0f);
    }

    for (PhysicPart* part : this->parts) {
        part->update(this, world, dt);
    }
}

void RigidBody::solve(float dt) {
    this->computeVelocity(dt);
    this->computeAngularVelocity(dt);

    for (int i = 0; i < JOINT_ITERATIONS; i++) {
        // Linear
        for (Joint* joint : this->joints) {
            if (!joint || !joint->getChild()) continue; 
            joint->solveLinear(this, dt);
            joint->getChild()->computeVelocity(dt);
            joint->getChild()->computeAngularVelocity(dt);
        }
        this->computeVelocity(dt);
        this->computeAngularVelocity(dt);

        // Angular
        for (Joint* joint : this->joints) {
            if (!joint || !joint->getChild()) continue; 
            joint->solveAngular(this, dt);
            joint->getChild()->computeAngularVelocity(dt);
        }
        this->computeAngularVelocity(dt);
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

glm::mat3 RigidBody::getInverseInertia() const {
    return this->inverse_inertia;
}

std::span<PhysicPart* const> RigidBody::getPhysicParts() const {
    return this->parts;
}

std::span<Joint* const> RigidBody::getJoints() const {
    return this->joints;
}

glm::vec3 RigidBody::getAcceleration() const {
    return this->acceleration;
}

glm::vec3 RigidBody::toLocalDir(const glm::vec3& dir) const {
    return phy::toLocalDir(this->orientation, dir);
}

glm::vec3 RigidBody::toGlobalDir(const glm::vec3& dir) const {
    return phy::toGlobalDir(this->orientation, dir);
}

glm::vec3 RigidBody::toLocalPos(const glm::dvec3& pos) const {
    return phy::toLocalPos(this->position, this->orientation, pos);
}

glm::dvec3 RigidBody::toGlobalPos(const glm::vec3& pos) const {
    return phy::toGlobalPos(this->position, this->orientation, pos);
}

glm::vec3 RigidBody::getGlobalVelocityAtLocal(const glm::vec3& pos) const {
    return this->velocity + glm::cross(toGlobalDir(angular_velocity), toGlobalDir(pos));
}

glm::vec3 RigidBody::getLocalVelocityAtLocal(const glm::vec3& pos) const {
    return toLocalDir(this->velocity) + glm::cross(this->angular_velocity, pos);
}

void RigidBody::addBodyForceAtBodyPoint(const glm::vec3& force, const glm::vec3& point) {
    this->force += toGlobalDir(force);
    this->torque += glm::cross(point, force);
}

void RigidBody::addWorldForceAtWorldPoint(const glm::vec3& force, const glm::dvec3& point) {
    this->force += force;
    this->torque += glm::cross(toLocalPos(point), toLocalDir(force));
}

void RigidBody::addBodyImpulseAtBodyPoint(const glm::vec3& impulse, const glm::vec3& point) {
    this->impulse += toGlobalDir(impulse);
    this->angular_impulse += glm::cross(point, impulse);
}

void RigidBody::addWorldImpulseAtWorldPoint(const glm::vec3& impulse, const glm::dvec3& point) {
    this->impulse += impulse;
    this->angular_impulse += glm::cross(toLocalPos(point), toLocalDir(impulse));
}

void RigidBody::addTorque(const glm::vec3& torque) {
    this->torque += torque;
}

void RigidBody::addBodyAngularImpulse(const glm::vec3& angular_impulse) {
    this->angular_impulse += angular_impulse;
}

void RigidBody::addWorldAngularImpulse(const glm::vec3& angular_impulse) {
    this->angular_impulse += toLocalDir(angular_impulse);
}