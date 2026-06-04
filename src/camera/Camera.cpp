#include "Camera.hpp"
#include "physics/Physics.hpp"

CameraTransform::CameraTransform(const glm::vec3& pos, bool orbit, bool relative): 
        pos(pos), orbit(orbit), relative(relative) {}

ViewData Camera::getViewData(float aspect) const {
    ViewData data;

    glm::quat final_rot = (this->transform.relative) ? (this->target_rot * this->dynamic_rot) : (this->dynamic_rot);

    data.camera_pos = phy::toGlobalPos(this->target_pos, this->target_rot, this->transform.pos + this->dynamic_pos);
    if (this->transform.orbit) {
        glm::vec3 front = final_rot * glm::vec3(0.0f, 0.0f, -1.0f);
        data.camera_pos -= glm::dvec3(this->distance * front);
    }

    glm::dmat4 rot_mat = glm::dmat4(glm::mat4_cast(glm::conjugate(final_rot)));
    glm::dmat4 pos_mat = glm::translate(glm::dmat4(1.0), -data.camera_pos);

    data.view_matrix = rot_mat * pos_mat;
    data.projection_matrix = glm::perspective(glm::radians(this->fov), aspect, Camera::MIN_DST, Camera::MAX_DST);

    return data;
}

void Camera::setTargetPosition(const glm::dvec3& pos) {
    this->target_pos = pos;
}

void Camera::setTargetRotation(const glm::quat& rot) {
    this->target_rot = rot;
}

void Camera::setCameraTransform(const CameraTransform& transform) {
    this->transform = transform;
}

void Camera::rotate(float yaw, float pitch) {
    glm::quat p = glm::angleAxis(glm::radians(-pitch), glm::vec3(1, 0, 0));
    glm::quat y = glm::angleAxis(glm::radians(-yaw), glm::vec3(0, 1, 0));
    this->dynamic_rot = glm::normalize(y * this->dynamic_rot * p);
}