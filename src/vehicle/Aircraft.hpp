#pragma once

#include "RigidBody.hpp"
#include "Wing.hpp"
#include "Engine.hpp"
#include "Hitbox.hpp"
#include "Camera.hpp"

#include <vector>

class Aircraft : public RigidBody {
protected:
    std::vector<Engine> engines; 
    std::vector<Wing> wings; 
    std::vector<Hitbox> hitboxes;

    Camera camera;
    float camera_distance = 7.5f;

public:
    virtual void update(float dt, World* world);
    void useCamera(GLuint shader, float aspect);

    void onMouseMove(float x, float y);
    void onMouseScroll(float s);

    const std::vector<Engine>& getEngines() const;
    const std::vector<Wing>& getWings() const;
};