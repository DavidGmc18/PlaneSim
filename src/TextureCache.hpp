#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <array>

class TextureCache {
    std::unordered_map<std::string, GLuint> cache;
    GLuint default_textures[4];

public:
    enum DefaultTex {
        DEFAULT_DIFFUSE,
        DEFAULT_SPECULAR,
        DEFAULT_NORMAL,
        DEFAULT_SHININESS
    };

    TextureCache();
    ~TextureCache();
    GLuint get(std::string path, bool invert = false);
    GLuint getDefault(DefaultTex tex);
};