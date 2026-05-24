#pragma once

#include "RigidBody.hpp"
#include "Model.hpp"
#include "control/AircraftControls.hpp"

class F16 : public RigidBody {
    Model model;
    glm::mat4 uModel;

    static constexpr float SCALE = 0.05f;

    struct {
        float flaperon_l, flaperon_r;
        float taileron_l, taileron_r;
        float rudder;
        float throttle;
    } fbw;

public:
    F16(glm::vec3 pos, TextureCache& cache);

    using RigidBody::update;
    void update(World* world, float dt, const AircraftControls* controls);

    void drawOpaque(GLuint shader);
    void drawTransparent(GLuint shader);
};