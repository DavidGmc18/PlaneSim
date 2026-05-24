#include "Material.hpp"
#include <iostream>

Material::Material(GLuint diffuse, GLuint specular, GLuint normal, GLuint glossiness, float opacity):
    diffuse(diffuse), specular(specular), normal(normal), glossiness(glossiness), opacity(opacity) {}

void Material::use(GLuint shader) const {
    if (this->diffuse == 0 || this->specular == 0 || this->normal == 0) {
        std::cerr << "Material is not properly initialized!\n";
        return;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->diffuse);
    glUniform1i(glGetUniformLocation(shader, "material.diffuse"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->specular);
    glUniform1i(glGetUniformLocation(shader, "material.specular"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, this->normal);
    glUniform1i(glGetUniformLocation(shader, "material.normal"), 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, this->glossiness);
    glUniform1i(glGetUniformLocation(shader, "material.glossiness"), 3);

    glUniform1f(glGetUniformLocation(shader, "material.opacity"), this->opacity);
}