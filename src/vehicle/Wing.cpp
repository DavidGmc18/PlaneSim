#include "Wing.hpp"
#include <iostream>

Airfoil::Airfoil(float min_alpha, float max_alpha, const std::vector<AirfoilSample> data)
    : min_alpha(min_alpha), max_alpha(max_alpha), data(std::move(data)) {}

AirfoilSample Airfoil::sample(float alpha) const {
    if (alpha <= min_alpha)
        return {data.front().cl, data.front().cd, data.front().cm};
    if (alpha >= max_alpha)
        return {data.back().cl, data.back().cd, data.back().cm};

    float range = max_alpha - min_alpha;
    float normalized_alpha = (alpha - min_alpha) / range;
    float index = normalized_alpha * (data.size() - 1);

    int i = index;
    float fraction = index - i;

    float cl = data[i].cl + fraction * (data[i+1].cl - data[i].cl);
    float cd = data[i].cd + fraction * (data[i+1].cd - data[i].cd);
    float cm = data[i].cm + fraction * (data[i+1].cm - data[i].cm);

    return {cl, cd, cm};
}


Wing::Wing(std::string name, const Airfoil* airfoil, const glm::vec3 center_of_pressure, glm::vec3 forward, glm::vec3 normal, float area, float chord)
    : name(name), airfoil(airfoil), center_of_pressure(center_of_pressure), forward(forward), normal(normal), area(area), chord(chord) {}

// TODO lateral drag
// TODO center of pressyre should be at 25%MAC
void Wing::apply_forces(RigidBody* rigid_body) {
    glm::vec3 lateral_dir = glm::normalize(glm::cross(this->forward, this->normal));
    glm::vec3 vel = rigid_body->getBodyVelocityAtPoint(this->center_of_pressure);
    if (glm::dot(vel, vel) < 0.0001f) return;

    float tas_forward = glm::dot(vel, this->forward);
    float tas_normal = glm::dot(vel, this->normal);
    // float tas_lateral = glm::dot(vel, lateral_dir);

    glm::vec3 vel_eff = tas_forward * this->forward + tas_normal * this->normal;
    float vel_eff_sq = glm::dot(vel_eff, vel_eff);
    this->v_eff = glm::sqrt(vel_eff_sq);
    if (vel_eff_sq < 0.0001f) return;

    glm::vec3 lift_dir = glm::normalize(glm::cross(lateral_dir, vel_eff));
    glm::vec3 drag_dir = glm::normalize(-vel_eff);

    this->alpha = glm::degrees(std::atan2(-tas_normal, tas_forward));
    if (!std::isnormal(alpha)) return;

    this->airfoil_sample = airfoil->sample(alpha);
    const float rho = 1.225f;
    float dynamic_pressure = 0.5f * rho * vel_eff_sq;

    glm::vec3 lift = lift_dir * (dynamic_pressure * this->area * this->airfoil_sample.cl);
    glm::vec3 drag = drag_dir * (dynamic_pressure * this->area * this->airfoil_sample.cd);
    glm::vec3 torque = lateral_dir * (dynamic_pressure * this->area * this->chord * this->airfoil_sample.cm);

    rigid_body->addBodyForceAtBodyPoint(lift + drag, this->center_of_pressure);    
    rigid_body->addTorque(torque);
}

std::string Wing::getName() const {
    return name;
}

float Wing::getAlpha() const {
    return alpha;
}

AirfoilSample Wing::getAirfoilSample() const {
    return this->airfoil_sample;
}

float Wing::getVeff() const {
    return this->v_eff;
}