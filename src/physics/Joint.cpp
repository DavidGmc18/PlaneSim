#include "Joint.hpp"
#include "Physics.hpp"

Joint::Joint(glm::vec3 pos, glm::quat rot): pos(pos), rot(rot) {}

void Joint::solveLinear(RigidBody* parent, float dt) {
    if (!parent || !this->child) return;
    if (dt == 0.0f) return;

    glm::quat parent_rot = parent->getPredictedOrientation(dt);
    glm::quat child_rot = this->child->getPredictedOrientation(dt);

    glm::dvec3 parent_pos = parent->getPredictedPosition(dt);
    glm::dvec3 child_pos = this->child->getPredictedPosition(dt);

    glm::vec3 lever_parent = glm::dvec3(phy::toGlobalDir(parent_rot, this->pos));
    glm::vec3 lever_child = glm::dvec3(phy::toGlobalDir(child_rot, this->child_joint_pos));

    glm::dvec3 parent_joint_pos = parent_pos + glm::dvec3(lever_parent);
    glm::dvec3 child_joint_pos = child_pos + glm::dvec3(lever_child);
    
    glm::vec3 joint_distance = glm::vec3(child_joint_pos - parent_joint_pos);
    glm::vec3 target_vel = joint_distance / dt;

    glm::mat3 parent_rot_matrix = glm::mat3_cast(parent_rot);
    glm::mat3 parent_inverse_inertia = parent_rot_matrix * parent->getInverseInertia() * glm::transpose(parent_rot_matrix);
    glm::mat3 child_rot_matrix = glm::mat3_cast(child_rot);
    glm::mat3 child_inverse_inertia = child_rot_matrix * this->child->getInverseInertia() * glm::transpose(child_rot_matrix);

    glm::mat3 parent_rx = glm::mat3(
         0.0f,           -lever_parent.z,  lever_parent.y,
         lever_parent.z,  0.0f,           -lever_parent.x,
        -lever_parent.y,  lever_parent.x,  0.0f
    );
    glm::mat3 child_rx = glm::mat3(
         0.0f,          -lever_child.z,  lever_child.y,
         lever_child.z,  0.0f,          -lever_child.x,
        -lever_child.y,  lever_child.x,  0.0f
    );

    glm::mat3 K = 
        glm::mat3((1.0f / parent->getMass()) + (1.0f / this->child->getMass()))
        -(parent_rx * parent_inverse_inertia * parent_rx)
        -(child_rx * child_inverse_inertia * child_rx);

    glm::vec3 impulse = glm::inverse(K) * target_vel;

    parent->addWorldImpulseAtWorldPoint(impulse, parent_joint_pos);
    this->child->addWorldImpulseAtWorldPoint(-impulse, child_joint_pos);
}

void Joint::solveAngular(RigidBody* parent, float dt) {
    if (!parent || !this->child) return;
    if (dt == 0.0f) return;

    glm::quat parent_rot = parent->getPredictedOrientation(dt);
    glm::quat child_rot = this->child->getPredictedOrientation(dt);

    glm::quat parent_anchor_world = parent_rot * this->rot;
    glm::quat child_anchor_world = child_rot  * this->child_joint_rot;
    glm::quat joint_rot = child_anchor_world * glm::conjugate(parent_anchor_world);

    glm::quat q = (joint_rot.w >= 0.0f) ? joint_rot : -joint_rot;
    float sin_half = glm::length(glm::vec3(q.x, q.y, q.z));
    if (sin_half < 1e-6f) return;

    glm::vec3 axis = glm::vec3(q.x, q.y, q.z) / sin_half;
    float angle = 2.0f * std::atan2(sin_half, q.w);

    glm::vec3 target_ang_vel = axis * (angle / dt);

    glm::mat3 parent_rot_matrix = glm::mat3_cast(parent_rot);
    glm::mat3 parent_inverse_inertia = parent_rot_matrix * parent->getInverseInertia() * glm::transpose(parent_rot_matrix);
    glm::mat3 child_rot_matrix = glm::mat3_cast(child_rot);
    glm::mat3 child_inverse_inertia = child_rot_matrix * this->child->getInverseInertia() * glm::transpose(child_rot_matrix);
    glm::mat3 effective_inertia = glm::inverse(parent_inverse_inertia + child_inverse_inertia);

    glm::vec3 angular_impulse = effective_inertia * target_ang_vel;

    parent->addWorldAngularImpulse(angular_impulse);
    this->child->addWorldAngularImpulse(-angular_impulse);
}

void Joint::connect(RigidBody* child, glm::vec3 child_joint_pos, glm::quat child_joint_rot) {
    this->child = child;
    this->child_joint_pos = child_joint_pos;
    this->child_joint_rot = child_joint_rot;
}

void Joint::disconnect() {
    this->child = NULL;
}

RigidBody* Joint::getChild() {
    return this->child;
}