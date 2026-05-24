#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <stb/stb_truetype.h>
#include <string>
#include <glm/glm.hpp>

class TextRenderer {
    inline static GLuint shader = 0;
    static constexpr int FIRST_CHAR = 32;
    static constexpr int CHAR_COUNT = 96;
    static constexpr int MAX_QUADS  = 4096;

    GLuint atlas;
    GLuint VAO, VBO;

    stbtt_bakedchar glyphs[96];

    glm::mat4 projection;

public:
    static void init();
    static void terminate();

    TextRenderer(const char* font_path, float pixel_height, float w, float h);
    ~TextRenderer();

    void onScreenResize(float w, float h);

    void render(const std::string& text, glm::vec2 pos, glm::vec4 color);
};