#pragma once

#include "Aircraft.hpp"
#include "model.hpp"

class F16 : public Aircraft {
    Model model;
    glm::mat4 uModel;

    static constexpr float SCALE = 0.05f;

public:
    F16(glm::vec3 pos, TextureCache& cache);

    void update(World* world, float dt) override;

    void drawOpaque(GLuint shader);
    void drawTransparent(GLuint shader);
};