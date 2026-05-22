#pragma once

#include "ControlAxis.hpp"
#include "KeyHandler.hpp"
#include <span>

// TODO input mapping
class AircraftControls: public KeyHandler {
public:
    enum AxisType {
        PITCH,
        ROLL,
        YAW,
        THROTTLE,
        AIRCRAFT_CONTROL_AXES_COUNT
    };

private:
    ControlAxis axes[AIRCRAFT_CONTROL_AXES_COUNT] = {
        /* PITCH */ ControlAxis(-1.0f, 1.0f),
        /* ROLL */ ControlAxis(-1.0f, 1.0f),
        /* YAW */ ControlAxis(-1.0f, 1.0f),
        /* THROTTLE */ ControlAxis(0.0f, 1.0f)
    };

    float left_trigger = 0.0f;
    float right_trigger = 0.0f;

public:
    void update(float dt);

    void onControllerAxis(const SDL_ControllerAxisEvent& event);
    void onKeyDown(SDL_Scancode code) override;
    void onKeyUp(SDL_Scancode code) override;

    float getAxisValue(AxisType axis) const;
    std::span<const ControlAxis> getAxes() const;
};