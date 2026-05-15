#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "RigidBody.hpp"
#include <string>

struct AirfoilSample {
    float cl, cd, cm;
};

// TODO this is very simplified
class Airfoil {
    const float min_alpha, max_alpha;
    std::vector<AirfoilSample> data;

public:
    Airfoil(float min_alpha, float max_alpha, const std::vector<AirfoilSample> data);
    AirfoilSample sample(float alpha) const;
};

class Wing {
    const Airfoil* airfoil;
    const glm::vec3 center_of_pressure;
    const glm::vec3 forward;
    const glm::vec3 normal;
    const float area;
    const float chord;

    // Debug
    const std::string name;
    float alpha = 0.0f;
    AirfoilSample airfoil_sample;
    float v_eff = 0.0f;

public:
    Wing(std::string name, const Airfoil* airfoil, const glm::vec3 center_of_pressure, glm::vec3 forward, glm::vec3 normal, float area, float chord);

    void apply_forces(RigidBody* rigid_body);

    std::string getName() const;
    float getAlpha() const;
    AirfoilSample getAirfoilSample() const;
    float getVeff() const;
};