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
    glm::vec3 pos{};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    float fov = 75.0f;

    // Free mode
    glm::vec3 speed{};

    // Orbit mode
    glm::vec3 target{};
    float distance = 7.5f;

public:
    CameraMode mode;
    Camera(CameraMode mode);

    void onMouseMove(float mx, float my);
    void onMouseScroll(float mz);

    void setSpeed(glm::vec3 speed);
    void setTarget(glm::vec3 target);

    void update(float dt);
    glm::mat4 getProjectionMatrix(float aspect_ratio) const;
    glm::mat4 getViewMatrix() const;
    glm::vec3 getPosition() const;
};