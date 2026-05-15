#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "RigidBody.hpp"
#include <string>

// TODO this is very simplified
class Airfoil {
    const float min_alpha, max_alpha;
    std::vector<glm::vec3> data;

public:
    Airfoil(const std::vector<glm::vec3> &data);
    glm::vec2 sample(float alpha) const;
};

class Wing {
    const Airfoil* airfoil;
    const glm::vec3 center_of_pressure;
    const glm::vec3 forward;
    const glm::vec3 normal;
    const float area;

    // Debug
    const std::string name;
    float alpha = 0.0f;
    float f_lift = 0.0f;
    float f_drag = 0.0f;

public:
    Wing(std::string name, const Airfoil* airfoil, const glm::vec3 center_of_pressure, glm::vec3 forward, glm::vec3 normal, float area);

    void apply_forces(RigidBody* rigid_body);

    std::string getName() const;
    float getAlpha() const;
    float getFlift() const;
    float getFdrag() const;
};

extern const Airfoil NACA_2412;