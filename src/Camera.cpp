#include "Camera.hpp"

Camera::Camera(glm::vec3 pos): pos(pos) {
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);
    right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
}

void Camera::update() {
    pitch = glm::clamp(pitch, -89.9f, 89.9f);
    fov = glm::clamp(fov, 1.0f, 150.0f);

    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);
    right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
}

void Camera::use(GLuint shader, float aspect) const {
    glm::mat4 view = glm::lookAt(pos, pos + front, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(fov), aspect, near, far); // TODO aspect

    glUniformMatrix4fv(glGetUniformLocation(shader, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniform3f(glGetUniformLocation(shader, "cameraPos"), pos.x, pos.y, pos.z);
}