#include "Camera.hpp"

Camera::Camera(CameraMode mode): mode(mode) {}

void Camera::onMouseMove(float mx, float my) {
    glm::quat p = glm::angleAxis(glm::radians(-my), glm::vec3(1, 0, 0));
    glm::quat y = glm::angleAxis(glm::radians(-mx), glm::vec3(0, 1, 0));
    this->rotation = glm::normalize(y * rotation * p);
}

void Camera::onMouseScroll(float mz) {
    switch (this->mode) {
        case CameraMode::FREE:
            this->fov -= 5.0f * mz;
            this->fov = glm::clamp(this->fov, 1.0f, 120.0f);
            break;

        case CameraMode::ORBIT:
            this->distance -= mz;
            this->distance = glm::clamp(this->distance, 1.0f, 256.0f);
            break;
    }
}

void Camera::setSpeed(glm::vec3 speed) {
    this->speed = speed;
}

void Camera::setTarget(glm::vec3 target) {
    this->target = target;
}

void Camera::update(float dt) {
    switch (this->mode) {
        case CameraMode::FREE:
            this->pos += dt * this->speed;
            break;

        case CameraMode::ORBIT:
            glm::vec3 front = this->rotation * glm::vec3(0.0f, 0.0f, -1.0f);
            this->pos = this->target - this->distance * front;
            break;
    }
}

glm::mat4 Camera::getProjectionMatrix(float aspect_ratio) const {
    return glm::perspective(glm::radians(this->fov), aspect_ratio, 0.001f, (float)(1<<20));
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::mat4_cast(glm::conjugate(this->rotation)) * glm::translate(glm::mat4(1.0f), -pos);
}

glm::vec3 Camera::getPosition() const {
    return this->pos;
}