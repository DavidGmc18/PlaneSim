#pragma once

#include <glm/glm.hpp>
#include "RigidBody.hpp"

class Joint {
    glm::vec3 pos;
    // TODO add joint rotation
    RigidBody* child;
    glm::vec3 relative_pos;
    glm::quat relative_orientation;

public:
    Joint(glm::vec3 pos, RigidBody* child = NULL);
    virtual ~Joint() = default;

    virtual void solve(RigidBody* parent, float dt);

    void setChild(RigidBody* child, glm::vec3 relative_pos, glm::quat relative_orientation);
    RigidBody* getChild();
};