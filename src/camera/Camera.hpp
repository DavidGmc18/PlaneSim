#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

enum class CameraMode {
    FREE,
    ORBIT
};

class Camera {
    glm::dvec3 pos{};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    float fov = 75.0f;

    // Free mode
    glm::vec3 speed{};

    // Orbit mode
    glm::dvec3 target{};
    float distance = 7.5f;

public:
    CameraMode mode;
    Camera(CameraMode mode);

    void onMouseMove(float mx, float my);
    void onMouseScroll(float mz);

    void setSpeed(glm::vec3 speed);
    void setTarget(glm::dvec3 target);

    void update(float dt);
    glm::dmat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspect_ratio) const;
    glm::dvec3 getPosition() const;
};