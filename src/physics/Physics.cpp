#include "Physics.hpp"
#include <glm/ext/quaternion_common.hpp>

glm::vec3 phy::toLocalDir(glm::quat rot, glm::vec3 dir) {
    return glm::conjugate(rot) * dir;
}

glm::vec3 phy::toGlobalDir(glm::quat rot, glm::vec3 dir) {
    return rot * dir;
}

glm::vec3 phy::toLocalPos(glm::dvec3 obj, glm::quat rot, glm::dvec3 pos) {
    return glm::conjugate(rot) * glm::vec3(pos - obj);
}

glm::dvec3 phy::toGlobalPos(glm::dvec3 obj, glm::quat rot, glm::vec3 pos) {
    return obj + glm::dvec3(rot * pos);
}