#pragma once

#include "VirtualAxis.hpp"
#include "ButtonHandler.hpp"
#include <span>
#include <SDL2/SDL.h>

// TODO input mapping
class AircraftControls {
public:
    enum AxisType {
        PITCH,
        ROLL,
        YAW,
        THROTTLE,
        AIRCRAFT_CONTROL_AXES_COUNT
    };

private:
    ButtonHandler<SDL_NUM_SCANCODES, SDL_Scancode> keys;
    ButtonHandler<SDL_CONTROLLER_BUTTON_MAX, SDL_GameControllerButton> controller_buttons;
    ButtonHandler<6, unsigned char>mouse_buttons;
    
    VirtualAxis axes[AIRCRAFT_CONTROL_AXES_COUNT] = {
        /* PITCH */ VirtualAxis(-1.0f, 1.0f),
        /* ROLL */ VirtualAxis(-1.0f, 1.0f),
        /* YAW */ VirtualAxis(-1.0f, 1.0f),
        /* THROTTLE */ VirtualAxis(0.0f, 1.0f)
    };

    float left_trigger = 0.0f;
    float right_trigger = 0.0f;

public:
    void update(float dt);

    void onControllerAxis(const SDL_ControllerAxisEvent& event);
    void onControllerButtonDown(SDL_GameControllerButton button);
    void onControllerButtonUp(SDL_GameControllerButton button);

    void onKeyDown(SDL_Scancode code);
    void onKeyUp(SDL_Scancode code);

    // void onMouseMotion();
    // void onMouseWheel();
    void onMouseButtonDown(unsigned char button);
    void onMouseButtonUp(unsigned char button);

    float getAxisValue(AxisType axis) const;
    std::span<const VirtualAxis> getAxes() const;
};