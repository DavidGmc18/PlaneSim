#pragma once

#include "PhysicPart.hpp"
#include <vector>
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

class Wing : public PhysicPart {
    const Airfoil* airfoil;
    const glm::vec3 center_of_pressure;
    const glm::vec3 forward;
    const glm::vec3 normal;
    const float area;
    const float chord;

    const float min_deflection, max_deflection;
    const float* deflection;

    // Debug
    const std::string name;
    float alpha = 0.0f;
    AirfoilSample airfoil_sample;
    float v_eff = 0.0f;

public:
    Wing(std::string name, const Airfoil* airfoil, const glm::vec3 center_of_pressure, glm::vec3 forward, glm::vec3 normal, float area, float chord, const float* deflection = NULL, float min_deflection = 0.0f, float max_deflection = 0.0f);

    void update(RigidBody* body, World* world, float dt) override;

    std::string getName() const;
    float getAlpha() const;
    AirfoilSample getAirfoilSample() const;
    float getVeff() const;
};