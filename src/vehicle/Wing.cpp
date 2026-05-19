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


Wing::Wing(std::string name, const Airfoil* airfoil, const glm::vec3 center_of_pressure, glm::vec3 forward, glm::vec3 normal,
    float area, float chord, const float* deflection, float min_deflection, float max_deflection)
    : name(name), airfoil(airfoil), center_of_pressure(center_of_pressure), forward(glm::normalize(forward)), normal(glm::normalize(normal)),
    area(area), chord(chord), deflection(deflection), min_deflection(min_deflection), max_deflection(max_deflection) {}

// TODO lateral drag
// TODO center of pressyre should be at 25%MAC
void Wing::update(RigidBody* body, World* world, float dt) {
    glm::vec3 forward_dir = this->forward;
    glm::vec3 normal_dir = this->normal;
    if (this->deflection != NULL && (this->min_deflection != 0.0f || this->max_deflection != 0.0f)) {
        glm::vec3 right_axis = glm::normalize(glm::cross(forward_dir, normal_dir));
        float deflection_rad = glm::radians(this->min_deflection + (*this->deflection / 2.0f + 0.5f) * (this->max_deflection - this->min_deflection));
        glm::mat4 deflection_mat = glm::rotate(glm::mat4(1.0f), deflection_rad, right_axis);

        forward_dir = glm::normalize(glm::vec3(deflection_mat * glm::vec4(forward_dir, 0.0f)));
        normal_dir = glm::normalize(glm::vec3(deflection_mat * glm::vec4(normal_dir, 0.0f)));
    }
    glm::vec3 lateral_dir = glm::normalize(glm::cross(forward_dir, normal_dir));

    glm::vec3 vel = body->getBodyVelocityAtPoint(this->center_of_pressure);
    if (glm::dot(vel, vel) < 0.0001f) return;

    float tas_forward = glm::dot(vel, forward_dir);
    float tas_normal = glm::dot(vel, normal_dir);
    // float tas_lateral = glm::dot(vel, lateral_dir);

    glm::vec3 vel_eff = tas_forward * forward_dir + tas_normal * normal_dir;
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

    body->addBodyForceAtBodyPoint(lift + drag, this->center_of_pressure);    
    body->addTorque(torque);
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