#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

class Camera {
    bool dirty;

    glm::vec3 pos;
    glm::vec3 front;
    glm::vec3 right;
    float yaw;
    float pitch;
    float fov;
    float near;
    float far;

    float aspect;
    glm::mat4 view;
    glm::mat4 projection;

public:
    Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = 0.0f, float pitch = 0.0f, float fov = 60.0f, float near = 0.1f, float far = 100.0f, float aspect = 1.0f) {
        this->pos = pos;
        this->yaw = yaw;
        this->pitch = pitch;
        this->fov = fov;
        this->near = near;
        this->far = far;
        this->aspect = aspect;
        dirty = true;
    }

    void update() {
        if (!dirty) return;

        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front);
        right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

        view = glm::lookAt(pos, pos + front, glm::vec3(0.0f, 1.0f, 0.0f));
        projection = glm::perspective(glm::radians(fov), aspect, near, far); // TODO aspect
        dirty = false;
    }

    void addYaw(float yaw) {
        this->yaw += yaw;
        dirty = true;
    }

    void addPitch(float pitch) {
        this->pitch += pitch;
        if (this->pitch > 89.9f)  this->pitch = 89.9f;
        if (this->pitch < -89.9f) this->pitch = -89.9f;
        dirty = true;
    }

    void addFov(float fov_offset) {
        this->fov += fov_offset;
        if (fov < 1.0f) fov = 1.0f;
        if (fov > 150.0f) fov = 150.0f;
        dirty = true;
    }

    void setAspect(float aspect) {
        this->aspect = aspect;
        dirty = true;
    }

    void move(glm::vec3 direction) {
        pos += front * direction.z;
        pos += right * direction.x;
        pos += glm::vec3(0.0f, 1.0f, 0.0f) * direction.y;
        dirty = true;
    }

    void use(GLuint shader) {
        update();
        glUniformMatrix4fv(glGetUniformLocation(shader, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader, "uView"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform3f(glGetUniformLocation(shader, "cameraPos"), pos.x, pos.y, pos.z);
    }
};