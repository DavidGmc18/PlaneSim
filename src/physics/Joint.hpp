#pragma once

#include <glm/glm.hpp>
#include "RigidBody.hpp"

class Joint {
    const glm::vec3 pos;
    const glm::quat rot;

    RigidBody* child = NULL;
    glm::vec3 child_joint_pos;
    glm::quat child_joint_rot;

public:
    Joint(glm::vec3 pos, glm::quat rot);
    virtual ~Joint() = default;

    virtual void solveLinear(RigidBody* parent, float dt);
    virtual void solveAngular(RigidBody* parent, float dt);

    void connect(RigidBody* child, glm::vec3 child_joint_pos, glm::quat child_joint_rot);
    void disconnect();

    RigidBody* getChild();
};