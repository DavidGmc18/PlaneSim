#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <span>
#include "world/World.hpp"

class PhysicPart;
class Joint;

class RigidBody {
    glm::vec3 force{}, impulse{}; // World-space
    glm::vec3 torque{}, angular_impulse{}; // Body-space

protected:
    glm::dvec3 position{}; // World-space
    glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // World-space
    glm::vec3 velocity{}; // World-space
    glm::vec3 angular_velocity{}; // Body-space
    glm::mat3 inertia{}, inverse_inertia{}; // Body-space
    float mass = 1.0f;
    bool gravity = true;

    std::vector<PhysicPart*> parts;
    std::vector<Joint*> joints;

    // Debug
    // glm::vec3 acceleration{}; // TODO

    // Constants
    static constexpr int JOINT_ITERATIONS = 4;

public:
    ~RigidBody();

    void computeVelocity(float dt);
    void computeAngularVelocity(float dt);

    glm::dvec3 getPredictedPosition(float dt) const;
    glm::quat getPredictedOrientation(float dt) const;

    virtual void update(World* world, float dt);
    virtual void solve(float dt);
    virtual void apply(float dt);

    glm::dvec3 getPosition() const;
    glm::quat getOrientation() const;
    glm::vec3 getVelocity() const;
    glm::vec3 getAngularVelocity() const;
    float getMass() const;
    glm::mat3 getInverseInertia() const;

    std::span<PhysicPart* const> getPhysicParts() const;
    std::span<Joint* const> getJoints() const;

    glm::vec3 toLocalDir(const glm::vec3& dir) const;
    glm::vec3 toGlobalDir(const glm::vec3& dir) const;

    glm::vec3 toLocalPos(const glm::dvec3& pos) const;
    glm::dvec3 toGlobalPos(const glm::vec3& pos) const;

    glm::vec3 getGlobalVelocityAtLocal(const glm::vec3& pos) const;
    glm::vec3 getLocalVelocityAtLocal(const glm::vec3& pos) const;

    void addBodyForceAtBodyPoint(const glm::vec3& force, const glm::vec3& point);
    void addWorldForceAtWorldPoint(const glm::vec3& force, const glm::dvec3& point);

    void addBodyImpulseAtBodyPoint(const glm::vec3& impulse, const glm::vec3& point);
    void addWorldImpulseAtWorldPoint(const glm::vec3& impulse, const glm::dvec3& point);

    void addTorque(const glm::vec3& torque);

    void addBodyAngularImpulse(const glm::vec3& angular_impulse);
    void addWorldAngularImpulse(const glm::vec3& angular_impulse);
};