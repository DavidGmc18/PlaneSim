#include "TextureCache.hpp"
#include <filesystem>
#include <iostream>

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

    GLenum format;
    switch (channels) {
        case 1: 
            format = GL_RED;
            if (invert) for (size_t i = 0; i < width * height * channels; i++) {
                data[i] = 255 - data[i];
            }
            break;

        case 3: 
            format = GL_RGB;
            if (invert) for (size_t i = 0; i < width * height * channels; i++) {
                data[i] = 255 - data[i];
            }
            break;

        case 4: 
            format = GL_RGBA;
            if (invert) for (size_t i = 0; i < width * height * channels; i += 4) {
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

TextureCache::TextureCache() {
    unsigned char default_diffuse[3] = {255, 255, 255};
    default_textures[DEFAULT_DIFFUSE] = gen_texture(GL_RGB, default_diffuse, 1, 1, GL_RGB);

    unsigned char default_specular[3] = {0, 0, 0};
    default_textures[DEFAULT_SPECULAR] = gen_texture(GL_RGB, default_specular, 1, 1, GL_RGB);

    unsigned char default_normal[3] = {128, 128, 255};
    default_textures[DEFAULT_NORMAL] = gen_texture(GL_RGB, default_normal, 1, 1, GL_RGB);

    unsigned char default_shininess[3] = {0, 0, 0};
    default_textures[DEFAULT_SHININESS] = gen_texture(GL_RGB, default_shininess, 1, 1, GL_RGB);
}

TextureCache::~TextureCache() {
    for (auto const& [path, texture] : cache)
        glDeleteTextures(1, &texture);
    for (int i = 0; i < (sizeof(default_textures) / sizeof(GLuint)); i++)
        glDeleteTextures(1, &default_textures[i]);
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

GLuint TextureCache::getDefault(DefaultTex tex) {
    if (tex < 0 || tex >= (sizeof(default_textures) / sizeof(GLuint))) return 0;
    return default_textures[tex];
}