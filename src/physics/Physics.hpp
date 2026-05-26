#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace phy {
    constexpr float STANDARD_GRAVITY = 9.80665f;

    glm::vec3 toLocalDir(glm::quat rot, glm::vec3 dir);
    glm::vec3 toGlobalDir(glm::quat rot, glm::vec3 dir);

    glm::vec3 toLocalPos(glm::dvec3 obj, glm::quat rot, glm::dvec3 pos);
    glm::dvec3 toGlobalPos(glm::dvec3 obj, glm::quat rot, glm::vec3 pos);
}