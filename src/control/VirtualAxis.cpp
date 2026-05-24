#include "VirtualAxis.hpp"
#include <glm/glm.hpp>

VirtualAxis::VirtualAxis(float min, float max): min(min), max(max) {}

void VirtualAxis::update(float dt) {
    this->absolute += relative * dt;
    if (this->decay) {
        this->absolute *= glm::exp(-decay * dt);
        if (glm::abs(this->absolute) <= 0.0001f) this->absolute = 0.0f;
    }
    this->absolute = glm::clamp(this->absolute, this->min, this->max);
}

void VirtualAxis::setAbsolute(float absolute) {
    this->absolute = absolute;
    this->relative = 0.0f;
    this->decay = 0.0f;
}