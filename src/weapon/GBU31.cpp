#include "GBU31.hpp"
#include "physics/Wing.hpp"
#include "physics/Hitbox.hpp"

const Airfoil FIN(-25.f, 25.0f, {
    /* -25° */ AirfoilSample(-0.90f, 0.120f, 0.0f),
    /* -20° */ AirfoilSample(-1.00f, 0.075f, 0.0f),
    /* -15° */ AirfoilSample(-0.95f, 0.045f, 0.0f),
    /* -10° */ AirfoilSample(-0.75f, 0.025f, 0.0f),
    /*  -5° */ AirfoilSample(-0.42f, 0.014f, 0.0f),
    /*   0° */ AirfoilSample( 0.00f, 0.011f, 0.0f),
    /*   5° */ AirfoilSample( 0.42f, 0.014f, 0.0f),
    /*  10° */ AirfoilSample( 0.75f, 0.025f, 0.0f),
    /*  15° */ AirfoilSample( 0.95f, 0.045f, 0.0f),
    /*  20° */ AirfoilSample( 1.00f, 0.075f, 0.0f),
    /*  25° */ AirfoilSample( 0.90f, 0.120f, 0.0f),
});

GBU31::GBU31(glm::dvec3 pos, TextureCache& cache): model("assets/GBU-31/GBU-31.obj", cache, true) {
    position = pos;

    mass = 925.0f;

    glm::mat3 inertia = glm::mat3(0.0f);
    inertia[0][0] = 280.0f;
    inertia[1][1] = 280.0f;
    inertia[2][2] = 18.0f;
    inverse_inertia = glm::inverse(inertia);

    parts.push_back(new Wing("Fin TL", &FIN, glm::vec3(-0.25f,  0.25f, 1.8f), glm::vec3(0, 0, -1), glm::vec3( 1,  1, 0), 0.18f, 0.3f));
    parts.push_back(new Wing("Fin TR", &FIN, glm::vec3( 0.25f,  0.25f, 1.8f), glm::vec3(0, 0, -1), glm::vec3(-1,  1, 0), 0.18f, 0.3f));
    parts.push_back(new Wing("Fin BL", &FIN, glm::vec3(-0.25f, -0.25f, 1.8f), glm::vec3(0, 0, -1), glm::vec3( 1, -1, 0), 0.18f, 0.3f));
    parts.push_back(new Wing("Fin BR", &FIN, glm::vec3( 0.25f, -0.25f, 1.8f), glm::vec3(0, 0, -1), glm::vec3(-1, -1, 0), 0.18f, 0.3f));

    parts.push_back(new Hitbox(glm::vec3(0.0f, 0.0f, -1.9f), 0.24f, 25000.0f, 60.0f));
    parts.push_back(new Hitbox(glm::vec3(0.0f, 0.0f,  0.0f), 0.24f, 40000.0f, 80.0f));
    parts.push_back(new Hitbox(glm::vec3(0.0f, 0.0f,  1.9f), 0.24f, 25000.0f, 60.0f));
}

void GBU31::apply(float dt) {
    RigidBody::apply(dt);

    uModel = glm::dmat4(1.0f);
    uModel = glm::translate(uModel, position);
    uModel *= glm::dmat4(glm::mat4_cast(orientation));
}

void GBU31::drawOpaque(GLuint shader, const glm::dmat4& view, const glm::mat4& projection) {
    model.drawOpaque(shader, uModel, view, projection);
}

void GBU31::drawTransparent(GLuint shader, const glm::dmat4& view, const glm::mat4& projection) {
    model.drawTransparent(shader, uModel, view, projection);
}