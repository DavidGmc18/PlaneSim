#define STB_TRUETYPE_IMPLEMENTATION
#include "TextRenderer.hpp"

#include "Shader.hpp"
#include <format>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <fstream>
#include <iostream>

void TextRenderer::init() {
    if (shader == 0)
        shader = compile_shader_program("shaders/text.vert", "shaders/text.frag");
}

void TextRenderer::terminate() {
    if (shader != 0) {
        glDeleteProgram(shader);
        shader = 0;
    }
}

TextRenderer::TextRenderer(const char* font_path, float pixel_height, float w, float h) {
    // Load font
    std::ifstream file(font_path, std::ios::binary | std::ios::ate);
    if (!file) throw std::runtime_error(std::format("Failed to open '{}'\n", font_path));
    size_t size = file.tellg();
    file.seekg(0);
    std::vector<unsigned char> font_data(size);
    file.read((char*)font_data.data(), size);

    // Baking
    std::vector<unsigned char> pixels(512 * 512);
    if (stbtt_BakeFontBitmap(
        font_data.data(), 0,
        pixel_height,
        pixels.data(), 512, 512,
        FIRST_CHAR, CHAR_COUNT,
        glyphs
    ) <= 0) std::cerr << "Atlas may be too small!\n";

    // Atlas
    glGenTextures(1, &atlas);
    glBindTexture(GL_TEXTURE_2D, atlas);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());

    // VAO & VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_QUADS * 6 * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW);  

    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(void*)(2 * sizeof(float)));

    // Projection
    onScreenResize(w, h);
}

TextRenderer::~TextRenderer() {
    glDeleteTextures(1, &atlas);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void TextRenderer::onScreenResize(float w, float h) {
    projection = glm::ortho(0.0f, w, h, 0.0f);
}

void TextRenderer::render(const std::string& text, glm::vec2 pos, glm::vec4 color) {
    std::vector<float> vertices;
    vertices.reserve(text.size() * 6 * 4);

    glm::vec2 cursor = pos;

    for (char c : text) {
        if (c < FIRST_CHAR || c >= FIRST_CHAR + CHAR_COUNT) continue;

        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(glyphs, 512, 512, c - FIRST_CHAR, &cursor.x, &cursor.y, &q, true);

        vertices.insert(vertices.end(), {
            q.x0, q.y0,  q.s0, q.t0,
            q.x1, q.y0,  q.s1, q.t0,
            q.x1, q.y1,  q.s1, q.t1,
            q.x0, q.y0,  q.s0, q.t0,
            q.x1, q.y1,  q.s1, q.t1,
            q.x0, q.y1,  q.s0, q.t1,
        });
    }

    if (vertices.empty()) return;

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform4fv(glGetUniformLocation(shader, "uColor"), 1, glm::value_ptr(color));
    glUniform1i(glGetUniformLocation(shader, "uAtlas"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlas);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 4);
}