#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class RigidBody {
    glm::vec3 force{}; // World-space
    glm::vec3 torque{}; // Body-space

protected:
    glm::vec3 position{}; // World-space
    glm::quat orientation{}; // World-space
    glm::vec3 velocity{}; // World-space
    glm::vec3 angular_velocity{}; // Body-space
    glm::mat3 inertia{}, inverse_inertia{}; // Body-space
    float mass = 1.0f;
    bool gravity = true;

public:
    virtual void update(float dt);

protected:
    glm::vec3 transformDirection(const glm::vec3& direction) const;
    glm::vec3 inverseTransformDirection(const glm::vec3& direction) const;
    glm::vec3 getVelocityAtPoint(const glm::vec3& point) const;

    void addForceAtPoint(const glm::vec3& force, const glm::vec3& point);
};