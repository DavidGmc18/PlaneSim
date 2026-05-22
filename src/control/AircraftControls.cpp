#include "AircraftControls.hpp"

void AircraftControls::update(float dt) {
    for (ControlAxis& axis : axes) {
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

void AircraftControls::onKeyDown(SDL_Scancode code) {
    switch (code) {
        // Pitch
        case SDL_SCANCODE_W:
            this->axes[PITCH].relative = 1.0f - this->keys[SDL_SCANCODE_S];
            this->axes[PITCH].decay = 0.0f;
            break;
        case SDL_SCANCODE_S:
            this->axes[PITCH].relative = -1.0f + this->keys[SDL_SCANCODE_W];
            this->axes[PITCH].decay = 0.0f;
            break;

        // Roll
        case SDL_SCANCODE_D:
            this->axes[ROLL].relative = 1.0f - this->keys[SDL_SCANCODE_A];
            this->axes[ROLL].decay = 0.0f;
            break;
        case SDL_SCANCODE_A:
            this->axes[ROLL].relative = -1.0f + this->keys[SDL_SCANCODE_D];
            this->axes[ROLL].decay = 0.0f;
            break;

        // Yaw
        case SDL_SCANCODE_E:
            this->axes[YAW].relative = 1.0f - this->keys[SDL_SCANCODE_Q];
            this->axes[YAW].decay = 0.0f;
            break;
        case SDL_SCANCODE_Q:
            this->axes[YAW].relative = -1.0f + this->keys[SDL_SCANCODE_E];
            this->axes[YAW].decay = 0.0f;
            break;

        // Throttle
        case SDL_SCANCODE_LSHIFT:
            this->axes[THROTTLE].relative = 1.0f - this->keys[SDL_SCANCODE_LCTRL];
            this->axes[THROTTLE].decay = 0.0f;
            break;
        case SDL_SCANCODE_LCTRL:
            this->axes[THROTTLE].relative = -1.0f + this->keys[SDL_SCANCODE_LSHIFT];
            this->axes[THROTTLE].decay = 0.0f;
            break;
    }
    KeyHandler::onKeyDown(code);
}

void AircraftControls::onKeyUp(SDL_Scancode code) {
    switch (code) {
        // Pitch
        case SDL_SCANCODE_W:
            this->axes[PITCH].relative = 0.0f - this->keys[SDL_SCANCODE_S];
            this->axes[PITCH].decay = 2.0f * !this->keys[SDL_SCANCODE_S];
            break;
        case SDL_SCANCODE_S:
            this->axes[PITCH].relative = 0.0f + this->keys[SDL_SCANCODE_W];
            this->axes[PITCH].decay = 2.0f * !this->keys[SDL_SCANCODE_W];
            break;

        // Roll
        case SDL_SCANCODE_D:
            this->axes[ROLL].relative = 0.0f - this->keys[SDL_SCANCODE_A];
            this->axes[ROLL].decay = 3.0f * !this->keys[SDL_SCANCODE_A];
            break;
        case SDL_SCANCODE_A:
            this->axes[ROLL].relative = 0.0f + this->keys[SDL_SCANCODE_D];
            this->axes[ROLL].decay = 3.0f * !this->keys[SDL_SCANCODE_D];
            break;

        // Yaw
        case SDL_SCANCODE_E:
            this->axes[YAW].relative = 0.0f - this->keys[SDL_SCANCODE_Q];
            this->axes[YAW].decay = 3.0f * !this->keys[SDL_SCANCODE_Q];
            break;
        case SDL_SCANCODE_Q:
            this->axes[YAW].relative = 0.0f + this->keys[SDL_SCANCODE_E];
            this->axes[YAW].decay = 3.0f * !this->keys[SDL_SCANCODE_E];
            break;

        // Throttle
        case SDL_SCANCODE_LSHIFT:
            this->axes[THROTTLE].relative = 0.0f - this->keys[SDL_SCANCODE_LCTRL];
            this->axes[THROTTLE].decay = 0.0f;
            break;
        case SDL_SCANCODE_LCTRL:
            this->axes[THROTTLE].relative = 0.0f + this->keys[SDL_SCANCODE_LSHIFT];
            this->axes[THROTTLE].decay = 0.0f;
            break;
    }

    KeyHandler::onKeyUp(code);
}

float AircraftControls::getAxisValue(AxisType axis) const {
    return this->axes[axis].absolute;
}

std::span<const ControlAxis> AircraftControls::getAxes() const {
    return this->axes;
}