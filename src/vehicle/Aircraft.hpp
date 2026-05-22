#pragma once

#include "RigidBody.hpp"
#include "Wing.hpp"
#include "Engine.hpp"
#include "Hitbox.hpp"
#include "Camera.hpp"
#include <vector>
#include <span>

class Aircraft : public RigidBody {
protected:
    Camera camera;
    float camera_distance = 7.5f;

public:
    ~Aircraft();
    virtual void update(World* world, float dt) override;
    void useCamera(GLuint shader, float aspect);

    // TODO
    void onMouseMove(float x, float y);
    void onMouseScroll(float s);
};