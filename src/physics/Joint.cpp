#include "Joint.hpp"
#include "Physics.hpp"

Joint::Joint(glm::vec3 pos, glm::quat rot): pos(pos), rot(rot) {}

void Joint::solve(RigidBody* parent, float dt) {
    if (!parent || !this->child) return;
    if (dt == 0.0f) return;

    glm::quat parent_rot = parent->getPredictedOrientation(dt);
    glm::quat child_rot = parent->getOrientation() * this->rot; //this->child->getPredictedOrientation(dt);

    glm::dvec3 parent_joint_pos = parent->getPredictedPosition(dt) + glm::dvec3(phy::toGlobalDir(parent_rot, this->pos));
    glm::dvec3 child_joint_pos = this->child->getPredictedPosition(dt) + glm::dvec3(phy::toGlobalDir(child_rot, this->child_joint_pos));

    // Distance
    glm::vec3 joint_distance = glm::vec3(child_joint_pos - parent_joint_pos);

    glm::vec3 target_vel = joint_distance / dt;
    float inverse_mass_parent = 1.0f / parent->getMass();
    float inverse_mass_child = 1.0f / this->child->getMass();

    glm::vec3 impulse = target_vel / (inverse_mass_parent + inverse_mass_child);

    glm::dvec3 pos_joint = parent->toGlobalPos(this->pos);
    parent->addWorldImpulseAtWorldPoint(impulse, pos_joint);
    this->child->addWorldImpulseAtWorldPoint(-impulse, pos_joint);

    // Angle
    *(glm::quat*)(&((char*)this->child)[80]) = parent->getOrientation() * this->rot;
    *(glm::vec3*)(&((char*)this->child)[108]) = glm::vec3(0);
}

void Joint::setChild(RigidBody* child, glm::vec3 child_joint_pos, glm::quat child_joint_rot) {
    this->child = child;
    this->child_joint_pos = child_joint_pos;
    this->child_joint_rot = child_joint_rot;
}

RigidBody* Joint::getChild() {
    return this->child;
}