#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "physics/RigidBody.hpp"

struct CameraTransform {
    glm::vec3 pos{};
    bool orbit = true; // 3rd person camera or 1st person camera
    bool relative = false; // Relative (entity) rotation or world rotation

    CameraTransform() = default;
    CameraTransform(const glm::vec3& pos, bool orbit, bool relative);
};

struct ViewData {
    glm::dvec3 camera_pos;
    glm::dmat4 view_matrix;
    glm::mat4 projection_matrix;
};

class Camera {
protected:
    static constexpr float MIN_DST = 0.001f;
    static constexpr float MAX_DST = 1<<20; // ~1024km
    float fov = 90.0f;

    glm::dvec3 target_pos{};
    glm::quat target_rot{};

    glm::vec3 dynamic_pos{};
    glm::quat dynamic_rot{};

    CameraTransform transform{};
    float distance = 10.0f; // For orbit mode

public:
    ViewData getViewData(float aspect) const;

    void setTargetPosition(const glm::dvec3& pos);
    void setTargetRotation(const glm::quat& rot);

    void setCameraTransform(const CameraTransform& transform);

    void rotate(float yaw, float pitch);
};