#include "Joint.hpp"

Joint::Joint(glm::vec3 pos, RigidBody* child): pos(pos), child(child) {}

// TODO does not handle rotation
void Joint::solve(RigidBody* parent, float dt) {
    if (!parent || !this->child) return;
    if (dt == 0.0f) return;

    // Distance
    glm::dvec3 parent_predicted_joint_pos = parent->getPredictedPosition(dt) + glm::dvec3(parent->toGlobalDir(this->pos));
    glm::dvec3 child_predicted_joint_pos = this->child->getPredictedPosition(dt) + glm::dvec3(this->child->toGlobalDir(this->relative_pos));
    glm::vec3 distance = glm::vec3(child_predicted_joint_pos - parent_predicted_joint_pos);

    glm::vec3 target_vel = distance / dt;
    float inverse_mass_parent = 1.0f / parent->getMass();
    float inverse_mass_child = 1.0f / this->child->getMass();

    glm::vec3 impulse = target_vel / (inverse_mass_parent + inverse_mass_child);

    glm::dvec3 pos_joint = parent->toGlobalPos(this->pos);
    parent->addWorldImpulseAtWorldPoint(impulse, pos_joint);
    this->child->addWorldImpulseAtWorldPoint(-impulse, pos_joint);

    // TODO also predict point location based on predicted angular velocity
}

void Joint::setChild(RigidBody* child, glm::vec3 relative_pos, glm::quat relative_orientation) {
    this->child = child;
    this->relative_pos = relative_pos;
    this->relative_orientation = relative_orientation;
}

RigidBody* Joint::getChild() {
    return this->child;
}