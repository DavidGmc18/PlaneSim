#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

struct Camera {
    glm::vec3 pos;
    glm::vec3 front;
    glm::vec3 right;
    float yaw = 0.0f;
    float pitch = 0.0f;
    float fov = 75.0f;
    float near = 0.001f;
    float far = 1024.0f;

    Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f));

    void update();
    void use(GLuint shader, float aspect) const;
};