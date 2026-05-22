#pragma once

struct ControlAxis {
    const float min = -1.0f, max = 1.0f;
    float absolute = 0.0f;
    float relative = 0.0f;
    float decay = 0.0f;

    ControlAxis() = default;
    ControlAxis(float min, float max);
    void update(float dt);

    void setAbsolute(float absolute);
};