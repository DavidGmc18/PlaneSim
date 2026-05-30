#pragma once

#include "entity/Entity.hpp"
#include "control/AircraftControls.hpp"

class AbstractAircraft : public Entity {
public:
    AbstractAircraft(Model model, glm::dvec3 pos = glm::dvec3(0), glm::quat rot = glm::quat(1, 0, 0, 0)): Entity(std::move(model), pos, rot) {}
    virtual ~AbstractAircraft() = default;

    virtual void control(const AircraftControls* controls) = 0;
};