#include "F16.hpp"

F16::F16(glm::vec3 pos, TextureCache& cache): model("assets/F-16/F-16.obj", cache, true) {
    position = pos;

    mass = 15000.0f;
    glm::mat3 inertia = glm::mat3(0.0f);
    inertia[0][0] = 150000.0f;
    inertia[1][1] = 200000.0f;
    inertia[2][2] = 40000.0f;
    inverse_inertia = glm::inverse(inertia);

    engines = {
        Engine(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), 15000.0f, 0.0f)
    };

    gravity = true;

    wings.push_back(Wing(&NACA_2412, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1), 20.0f));

    hitboxes.push_back(Hitbox(glm::vec3( 0.0f, -0.835f,  2.4f), 0.110f, 500000.0f, 1600.0f));
    hitboxes.push_back(Hitbox(glm::vec3( 1.0f, -0.780f, -1.0f), 0.165f, 500000.0f, 3200.0f));
    hitboxes.push_back(Hitbox(glm::vec3(-1.0f, -0.780f, -1.0f), 0.165f, 500000.0f, 3200.0f));
}

void F16::update(float dt, World* world) {
    Aircraft::update(dt, world);

    uModel = glm::mat4(1.0f);
    uModel = glm::translate(uModel, position);
    uModel *= glm::mat4_cast(orientation);
    uModel = glm::rotate(uModel, glm::radians(-90.0f), glm::vec3(0, 1, 0));
    uModel = glm::scale(uModel, glm::vec3(SCALE));
}

void F16::drawOpaque(GLuint shader) {
    model.drawOpaque(shader, uModel);
}

void F16::drawTransparent(GLuint shader) {
    model.drawTransparent(shader, uModel);
}