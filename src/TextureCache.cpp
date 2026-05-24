#include "TextureCache.hpp"
#include <filesystem>
#include <iostream>
#include <format>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

static GLuint gen_texture(GLint internalformat, unsigned char *data, int width, int height, GLenum format) {
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    return id;
}

static GLuint load_texture(std::string path, bool invert) {
    int width, height, channels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Failed to load texture " << path << '\n';
        return 0;
    }

    size_t values = (size_t)width * height * channels;

    GLenum format;
    switch (channels) {
        case 1:
            format = GL_RED;
            if (invert) for (size_t i = 0; i < values; i++) {
                data[i] = 255 - data[i];
            }
            break;

        case 3: 
            format = GL_RGB;
            if (invert) for (size_t i = 0; i < values; i++) {
                data[i] = 255 - data[i];
            }
            break;

        case 4: 
            format = GL_RGBA;
            if (invert) for (size_t i = 0; i < values; i += 4) {
                data[i] = 255 - data[i];
                data[i+1] = 255 - data[i+1];
                data[i+2] = 255 - data[i+2];
            }
            break;

        default:
            std::cerr << "Failed to load texture " << path << '\n';
            stbi_image_free(data);
            return 0;
    }

        GLuint id = gen_texture(GL_RGB, data, width, height, format);

    stbi_image_free(data);
    return id;
}

TextureCache::~TextureCache() {
    for (auto const& [path, texture] : cache)
        glDeleteTextures(1, &texture);
    cache.clear();
}

GLuint TextureCache::get(std::string path, bool invert) {
    path = std::filesystem::absolute(path);
    std::string key = path;
    if (invert) key += ":INVERTED";
    auto it = cache.find(key);

    if (it != cache.end())
        return it->second;

    GLuint texture = load_texture(path, invert);
    if (texture)
        cache[key] = texture;

    return texture;
}

GLuint TextureCache::getColor(glm::vec3 color) {
    glm::u8vec3 u8_color = glm::u8vec3(color * glm::vec3(255));

    std::string key = std::format(":{}:{}:{}", u8_color.r, u8_color.g, u8_color.b);
    auto it = cache.find(key);

    if (it != cache.end())
        return it->second;

    unsigned char data[3] = {u8_color.r, u8_color.g, u8_color.b};
    GLuint texture = gen_texture(GL_RGB, data, 1, 1, GL_RGB);
    cache[key] = texture;

    return texture;
}

GLuint TextureCache::getScalar(float value) {
    unsigned char u8_value = (unsigned char)(value * 255);

    std::string key = ":" + u8_value;
    auto it = cache.find(key);

    if (it != cache.end())
        return it->second;

    unsigned char data[1] = {u8_value};
    GLuint texture = gen_texture(GL_RED, data, 1, 1, GL_RED);
    cache[key] = texture;

    return texture;
}