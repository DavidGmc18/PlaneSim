#pragma once

#include <glad/glad.h>

struct Material {
    GLuint diffuse = 0;
    GLuint specular = 0;
    GLuint normal = 0;
    GLuint glossiness = 0;
    float opacity = 1.0f;

    Material() = default;
    Material(GLuint diffuse, GLuint specular, GLuint normal, float glossiness, float opacity);

    void use(GLuint shader) const;
};