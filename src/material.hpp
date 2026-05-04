#pragma once

#include <glad/glad.h>
#include <algorithm>
#include <iostream>

struct Material {
    GLuint diffuse = 0;
    GLuint specular = 0;
    GLuint normal = 0;
    GLuint shininess = 0; // TODO rename to glossiness
    float opacity = 1.0f;

    Material() = default;

    Material(GLuint diffuse, GLuint specular, GLuint normal, float shininess, float opacity):
        diffuse(diffuse), specular(specular), shininess(shininess), normal(normal), opacity(opacity) {}

    void use(GLuint shader) const {
        if (diffuse == 0 || specular == 0 || normal == 0) {
            std::cerr << "Material is not properly initialized!\n";
            return;
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse);
        glUniform1i(glGetUniformLocation(shader, "material.diffuse"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular);
        glUniform1i(glGetUniformLocation(shader, "material.specular"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normal);
        glUniform1i(glGetUniformLocation(shader, "material.normal"), 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, shininess);
        glUniform1i(glGetUniformLocation(shader, "material.shininess"), 3);

        glUniform1f(glGetUniformLocation(shader, "material.opacity"), opacity);
    }
};