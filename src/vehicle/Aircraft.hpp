#pragma once

#include "RigidBody.hpp"
#include "Wing.hpp"
#include "Engine.hpp"
#include "Hitbox.hpp"
#include "Camera.hpp"
#include <vector>
#include <span>

enum AircraftControls {
    PITCH = 0,
    ROLL,
    YAW,
    THROTTLE,
    AIRCRAFT_CONTROLS_COUNT
};

class AircraftControl {
    const float min, max;
    float value = 0.0f;
    float relative = 0.0f;
public:
    AircraftControl(float min, float max);
    void setAbsolute(float value);
    void setRelative(float relative);
    void update(float dt);
    const float* get() const;
};

class Aircraft : public RigidBody {
protected:
    Camera camera;
    float camera_distance = 7.5f;

    AircraftControl controls[AIRCRAFT_CONTROLS_COUNT] = {
        /* PITCH */ AircraftControl(-1.0f, 1.0f),
        /* ROLL */ AircraftControl(-1.0f, 1.0f),
        /* YAW */ AircraftControl(-1.0f, 1.0f),
        /* THROTTLE */ AircraftControl( 0.0f, 1.0f)
    };

public:
    ~Aircraft();
    virtual void update(World* world, float dt) override;
    void useCamera(GLuint shader, float aspect);

    void onMouseMove(float x, float y);
    void onMouseScroll(float s);

    void onJoyMotion(float value, AircraftControls control, bool relative = false);

    std::span<const AircraftControl> getControls() const;
};