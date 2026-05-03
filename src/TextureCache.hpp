#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <array>

class TextureCache {
    std::unordered_map<std::string, GLuint> cache;
    GLuint default_textures[3];

public:
    enum DefaultTex {
        DEFAULT_DIFFUSE,
        DEFAULT_SPECULAR,
        DEFAULT_NORMAL
    };

    TextureCache();
    ~TextureCache();
    GLuint get(std::string path);
    GLuint getDefault(DefaultTex tex);
};