#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

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

public:
    virtual void update(float dt);

    glm::vec3 getPosition() const;
    glm::quat getOrientation() const;
    glm::vec3 getVelocity() const;
    glm::vec3 getAngularVelocity() const;
    float getMass() const;

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
};