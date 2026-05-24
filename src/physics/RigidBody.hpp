#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <span>
#include "world/World.hpp"

class PhysicPart;

class RigidBody {
    glm::vec3 force{}, impulse{}; // World-space
    glm::vec3 torque{}, angular_impulse{}; // Body-space

protected:
    glm::vec3 position{}; // World-space
    glm::quat orientation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // World-space
    glm::vec3 velocity{}; // World-space
    glm::vec3 angular_velocity{}; // Body-space
    glm::mat3 inertia{}, inverse_inertia{}; // Body-space
    float mass = 1.0f;
    bool gravity = true;

    std::vector<PhysicPart*> parts;

    // Debug
    glm::vec3 acceleration{};

public:
    ~RigidBody();

    virtual void update(World* world, float dt);

    glm::vec3 getPosition() const;
    glm::quat getOrientation() const;
    glm::vec3 getVelocity() const;
    glm::vec3 getAngularVelocity() const;
    float getMass() const;

    std::span<const PhysicPart* const> getPhysicParts() const;

    glm::vec3 getAcceleration() const;

    glm::vec3 toWorldDirection(const glm::vec3& direction) const;
    glm::vec3 toBodyDirection(const glm::vec3& direction) const;

    glm::vec3 toWorldPos(const glm::vec3& point) const;
    glm::vec3 toBodyPos(const glm::vec3& point) const;

    glm::vec3 getBodyVelocityAtPoint(const glm::vec3& point) const;
    glm::vec3 getWorldVelocityAtPoint(const glm::vec3& point) const;

    void addBodyForceAtBodyPoint(const glm::vec3& force, const glm::vec3& point);
    void addWorldForceAtWorldPoint(const glm::vec3& force, const glm::vec3& point);

    void addBodyImpulseAtBodyPoint(const glm::vec3& impulse, const glm::vec3& point);
    void addWorldImpulseAtWorldPoint(const glm::vec3& impulse, const glm::vec3& point);

    void addTorque(const glm::vec3& torque);
};