#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <array>
#include <glm/glm.hpp>

class TextureCache {
    std::unordered_map<std::string, GLuint> cache;

public:
    ~TextureCache();
    GLuint get(std::string path, bool invert = false);
    GLuint getColor(glm::vec3 color);
    GLuint getScalar(float value);
};