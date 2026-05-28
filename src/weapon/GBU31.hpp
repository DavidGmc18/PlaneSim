#pragma once

#include "physics/RigidBody.hpp"
#include "rendering/Model.hpp"

class GBU31 : public RigidBody {
    Model model;
    glm::dmat4 uModel;

public:
    GBU31(glm::dvec3 pos, TextureCache& cache);

    void apply(float dt) override;

    void drawOpaque(GLuint shader, const glm::dmat4& view, const glm::mat4& projection);
    void drawTransparent(GLuint shader, const glm::dmat4& view, const glm::mat4& projection);
};