#pragma once

#include "physics/RigidBody.hpp"
#include "rendering/Model.hpp"

class Entity : public RigidBody {
    Model model; // TODO make model shader per instance of class;
    glm::dmat4 model_matrix;

public:
    Entity(Model model, glm::dvec3 pos = glm::dvec3(0), glm::quat rot = glm::quat(1, 0, 0, 0));

    void apply(float dt) override;

    void drawOpaque(GLuint shader, const glm::dmat4& view, const glm::mat4& projection);
    void drawTransparent(GLuint shader, const glm::dmat4& view, const glm::mat4& projection);
};