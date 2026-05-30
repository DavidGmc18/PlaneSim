#pragma once

#include "AbstractAircraft.hpp"

class F16 : public AbstractAircraft {
    struct {
        float flaperon_l, flaperon_r;
        float taileron_l, taileron_r;
        float rudder;
        float throttle;
    } fbw;

public:
    F16(TextureCache& cache, glm::dvec3 pos = glm::dvec3(0), glm::quat rot = glm::quat(1, 0, 0, 0));

    void control(const AircraftControls* controls) override;
};