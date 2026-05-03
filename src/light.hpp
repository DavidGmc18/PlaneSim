#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>

struct ParallelLight {
    glm::vec3 dir;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    ParallelLight(glm::vec3 dir, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular):
        dir(dir), ambient(ambient), diffuse(diffuse), specular(specular) {}

    void use(GLuint shader) {
        dir = glm::normalize(dir);
        glUniform3f(glGetUniformLocation(shader, "parallelLight.dir"), dir.x, dir.y, dir.z);
        glUniform3f(glGetUniformLocation(shader, "parallelLight.ambient"), ambient.x, ambient.y, ambient.z);
        glUniform3f(glGetUniformLocation(shader, "parallelLight.diffuse"), diffuse.x, diffuse.y, diffuse.z);
        glUniform3f(glGetUniformLocation(shader, "parallelLight.specular"), specular.x, specular.y, specular.z);
    }
};

struct Light {
    glm::vec3 pos;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    Light(glm::vec3 pos, glm::vec3 ambient, glm::vec3 diffuse,glm::vec3 specular):
        pos(pos), ambient(ambient), diffuse(diffuse), specular(specular) {}

    void use(GLuint shader, int idx) {
        std::string base = "lights[" + std::to_string(idx) + "].";
        glUniform3f(glGetUniformLocation(shader, (base + "pos").c_str()), pos.x, pos.y, pos.z);
        glUniform3f(glGetUniformLocation(shader, (base + "ambient").c_str()), ambient.x, ambient.y, ambient.z);
        glUniform3f(glGetUniformLocation(shader, (base + "diffuse").c_str()), diffuse.x, diffuse.y, diffuse.z);
        glUniform3f(glGetUniformLocation(shader, (base + "specular").c_str()), specular.x, specular.y, specular.z);
    }

    static void setCount(GLuint shader, int count) {
        glUniform1i(glGetUniformLocation(shader, "lightCount"), count);
    }
};
