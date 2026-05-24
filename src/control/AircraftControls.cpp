#include "AircraftControls.hpp"

void AircraftControls::update(float dt) {
    for (VirtualAxis& axis : axes) {
        axis.update(dt);
    }
}

void AircraftControls::onControllerAxis(const SDL_ControllerAxisEvent& event) {
    switch (event.axis) {
        case SDL_CONTROLLER_AXIS_LEFTY:
            this->axes[PITCH].setAbsolute(-(float)event.value / 32768);
            break;

        case SDL_CONTROLLER_AXIS_LEFTX:
            this->axes[ROLL].setAbsolute((float)event.value / 32768);
            break;

        case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
            this->left_trigger = (float)event.value / 32767;
            this->axes[YAW].setAbsolute(right_trigger - left_trigger);
            break;

        case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
            this->right_trigger = (float)event.value / 32767;
                this->axes[YAW].setAbsolute(right_trigger - left_trigger);
            break;

        case  SDL_CONTROLLER_AXIS_RIGHTY:
            float value = -(float)event.value / 32768;
            if (std::abs(value) < 0.075f) value = 0.0f; // TODO
            this->axes[THROTTLE].relative = value;
            this->axes[THROTTLE].decay = 0.0f;
            break;
    }
}

void AircraftControls::onControllerButtonDown(SDL_GameControllerButton button) {
    this->controller_buttons.onButtonDown(button);
}

void AircraftControls::onControllerButtonUp(SDL_GameControllerButton button) {
    this->controller_buttons.onButtonUp(button);
}

#define AXIS_KEYS_DOWN(AXIS, DOWN_KEY, UP_KEY, RATE) \
    case (UP_KEY): \
            this->axes[(AXIS)].relative = (RATE) - (RATE) * this->keys[(DOWN_KEY)]; \
            this->axes[(AXIS)].decay = 0.0f; \
            break; \
        case (DOWN_KEY): \
            this->axes[(AXIS)].relative = -(RATE) + (RATE) * this->keys[(UP_KEY)]; \
            this->axes[(AXIS)].decay = 0.0f; \
            break;

#define AXIS_KEYS_UP(AXIS, DOWN_KEY, UP_KEY, RATE, DECAY) \
    case (UP_KEY): \
        this->axes[(AXIS)].relative = 0.0f - (RATE) * this->keys[(DOWN_KEY)]; \
        this->axes[(AXIS)].decay = (DECAY) * !this->keys[(DOWN_KEY)]; \
        break; \
    case (DOWN_KEY): \
        this->axes[(AXIS)].relative = 0.0f + (RATE) * this->keys[(UP_KEY)]; \
        this->axes[(AXIS)].decay = (DECAY) * !this->keys[(UP_KEY)]; \
        break;

void AircraftControls::onKeyDown(SDL_Scancode code) {
    switch (code) {
        AXIS_KEYS_DOWN(PITCH, SDL_SCANCODE_S, SDL_SCANCODE_W, 1.0f);
        AXIS_KEYS_DOWN(ROLL, SDL_SCANCODE_A, SDL_SCANCODE_D, 1.0f);
        AXIS_KEYS_DOWN(YAW, SDL_SCANCODE_Q, SDL_SCANCODE_E, 1.0f);
        AXIS_KEYS_DOWN(THROTTLE, SDL_SCANCODE_LCTRL, SDL_SCANCODE_LSHIFT, 1.0f);
        default: break;
    }

    this->keys.onButtonDown(code);
}

void AircraftControls::onKeyUp(SDL_Scancode code) {
    switch (code) {
        AXIS_KEYS_UP(PITCH, SDL_SCANCODE_S, SDL_SCANCODE_W, 1.0f, 2.0f);
        AXIS_KEYS_UP(ROLL, SDL_SCANCODE_A, SDL_SCANCODE_D, 1.0f, 2.0f);
        AXIS_KEYS_UP(YAW, SDL_SCANCODE_Q, SDL_SCANCODE_E, 1.0f, 2.0f);
        AXIS_KEYS_UP(THROTTLE, SDL_SCANCODE_LCTRL, SDL_SCANCODE_LSHIFT, 1.0f, 0.0f)
        default: break;
    }

    this->keys.onButtonUp(code);
}

void AircraftControls::onMouseButtonDown(unsigned char button) {
    this->mouse_buttons.onButtonDown(button);
}

void AircraftControls::onMouseButtonUp(unsigned char button) {
    this->mouse_buttons.onButtonUp(button);
}

float AircraftControls::getAxisValue(AxisType axis) const {
    return this->axes[axis].absolute;
}

std::span<const VirtualAxis> AircraftControls::getAxes() const {
    return this->axes;
}