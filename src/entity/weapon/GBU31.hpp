#pragma once

#include "entity/Entity.hpp"

class GBU31 : public Entity {
public:
    GBU31(TextureCache& cache, glm::dvec3 pos = glm::dvec3(0), glm::quat rot = glm::quat(1, 0, 0, 0));
};