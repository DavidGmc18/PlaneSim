#pragma once

#include "physics/RigidBody.hpp"
#include "rendering/Model.hpp"

class JDAM : public RigidBody {
    Model model;
    glm::dmat4 uModel;

    static constexpr float SCALE = 0.5f;

public:
    JDAM(glm::dvec3 pos, TextureCache& cache);

    using RigidBody::update;
    void update(World* world, float dt);

    void drawOpaque(GLuint shader, const glm::dmat4& view, const glm::mat4& projection);
    void drawTransparent(GLuint shader, const glm::dmat4& view, const glm::mat4& projection);
};