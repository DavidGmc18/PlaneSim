#pragma once

#include "physics/RigidBody.hpp"
#include "rendering/Model.hpp"
#include "control/AircraftControls.hpp"

class F16 : public RigidBody {
    Model model;
    glm::dmat4 uModel;

    static constexpr float SCALE = 0.05f;

    struct {
        float flaperon_l, flaperon_r;
        float taileron_l, taileron_r;
        float rudder;
        float throttle;
    } fbw;

public:
    F16(glm::dvec3 pos, TextureCache& cache);

    using RigidBody::update;
    void update(World* world, float dt, const AircraftControls* controls);

    void drawOpaque(GLuint shader, const glm::dmat4& view, const glm::mat4& projection);
    void drawTransparent(GLuint shader, const glm::dmat4& view, const glm::mat4& projection);
};