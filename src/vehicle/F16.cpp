#include "F16.hpp"

const Airfoil MAIN_WING(-20.0f, 30.0f, {
    /* -20° */ AirfoilSample(-0.82f, 0.085f,  0.060f),
    /* -18° */ AirfoilSample(-0.90f, 0.065f,  0.052f),
    /* -16° */ AirfoilSample(-0.98f, 0.050f,  0.044f),
    /* -14° */ AirfoilSample(-1.02f, 0.038f,  0.034f),
    /* -12° */ AirfoilSample(-1.00f, 0.028f,  0.022f),
    /* -10° */ AirfoilSample(-0.88f, 0.020f,  0.012f),
    /*  -8° */ AirfoilSample(-0.68f, 0.015f,  0.004f),
    /*  -6° */ AirfoilSample(-0.46f, 0.012f, -0.002f),
    /*  -4° */ AirfoilSample(-0.24f, 0.010f, -0.008f),
    /*  -2° */ AirfoilSample(-0.02f, 0.009f, -0.012f),
    /*   0° */ AirfoilSample( 0.20f, 0.009f, -0.014f),
    /*   2° */ AirfoilSample( 0.42f, 0.010f, -0.015f),
    /*   4° */ AirfoilSample( 0.62f, 0.012f, -0.016f),
    /*   6° */ AirfoilSample( 0.82f, 0.015f, -0.017f),
    /*   8° */ AirfoilSample( 1.00f, 0.020f, -0.018f),
    /*  10° */ AirfoilSample( 1.16f, 0.027f, -0.019f),
    /*  12° */ AirfoilSample( 1.28f, 0.037f, -0.020f),
    /*  14° */ AirfoilSample( 1.36f, 0.052f, -0.022f),
    /*  16° */ AirfoilSample( 1.38f, 0.072f, -0.026f),
    /*  18° */ AirfoilSample( 1.30f, 0.100f, -0.030f),
    /*  20° */ AirfoilSample( 1.15f, 0.138f, -0.038f),
    /*  22° */ AirfoilSample( 0.98f, 0.180f, -0.048f),
    /*  24° */ AirfoilSample( 0.82f, 0.228f, -0.058f),
    /*  26° */ AirfoilSample( 0.68f, 0.280f, -0.062f),
    /*  28° */ AirfoilSample( 0.55f, 0.335f, -0.060f),
    /*  30° */ AirfoilSample( 0.44f, 0.390f, -0.055f)
});

const Airfoil HORIZONTAL_STAB(-20.f, 25.0f, {
    /* -20° */ AirfoilSample(-0.95f, 0.060f, 0.0f),
    /* -15° */ AirfoilSample(-1.05f, 0.040f, 0.0f),
    /* -10° */ AirfoilSample(-0.80f, 0.020f, 0.0f),
    /*  -5° */ AirfoilSample(-0.45f, 0.012f, 0.0f),
    /*   0° */ AirfoilSample( 0.10f, 0.010f, 0.0f),
    /*   5° */ AirfoilSample( 0.55f, 0.014f, 0.0f),
    /*  10° */ AirfoilSample( 0.90f, 0.024f, 0.0f),
    /*  15° */ AirfoilSample( 1.10f, 0.042f, 0.0f),
    /*  20° */ AirfoilSample( 1.05f, 0.072f, 0.0f),
    /*  25° */ AirfoilSample( 0.85f, 0.115f, 0.0f)
});

const Airfoil VERTICAL_STAB(-20.f, 20.0f, {
    /* -20° */ AirfoilSample(-0.70f, 0.055f, 0.0f),
    /* -15° */ AirfoilSample(-0.85f, 0.038f, 0.0f),
    /* -10° */ AirfoilSample(-0.72f, 0.022f, 0.0f),
    /*  -5° */ AirfoilSample(-0.40f, 0.013f, 0.0f),
    /*   0° */ AirfoilSample( 0.00f, 0.010f, 0.0f),
    /*   5° */ AirfoilSample( 0.40f, 0.013f, 0.0f),
    /*  10° */ AirfoilSample( 0.72f, 0.022f, 0.0f),
    /*  15° */ AirfoilSample( 0.85f, 0.038f, 0.0f),
    /*  20° */ AirfoilSample( 0.70f, 0.055f, 0.0f)
});

F16::F16(glm::vec3 pos, TextureCache& cache): model("assets/F-16/F-16.obj", cache, true) {
    position = pos;

    mass = 9297.0f;

    glm::mat3 inertia = glm::mat3(0.0f);
    inertia[0][0] = 150000.0f;
    inertia[1][1] = 200000.0f;
    inertia[2][2] = 40000.0f;
    inverse_inertia = glm::inverse(inertia);

    engines = {
        Engine(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), 64900.0f, 0.0f)
    };

    gravity = true;

    wings.push_back(Wing("Left wing", &MAIN_WING, glm::vec3(-5, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), 13.935f, 3.45f, &this->controls.x, -10, 10));
    wings.push_back(Wing("Right wing", &MAIN_WING, glm::vec3(5, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), 13.935f, 3.45f, &this->controls.x, 10, -10));

    wings.push_back(Wing("Horizontal stabilizer L", &HORIZONTAL_STAB, glm::vec3(-2.5f, 0, -7.0f), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), 5.9f, 1.9f, &this->controls.y, -20, 20));
    wings.push_back(Wing("Horizontal stabilizer R", &HORIZONTAL_STAB, glm::vec3(2.5f, 0, -7.0f), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), 5.9f, 1.9f, &this->controls.y, -20, 20));

    wings.push_back(Wing("Vertical stabilizer", &VERTICAL_STAB, glm::vec3(0.0f, 1.5f, -6.8f), glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), 6.0f, 2.4f, &this->controls.z, -10, 10));

    hitboxes.push_back(Hitbox(glm::vec3( 0.0f, -0.835f,  2.4f), 0.110f, 500000.0f, 1000.0f));
    hitboxes.push_back(Hitbox(glm::vec3( 1.0f, -0.780f, -1.0f), 0.165f, 500000.0f, 3000.0f));
    hitboxes.push_back(Hitbox(glm::vec3(-1.0f, -0.780f, -1.0f), 0.165f, 500000.0f, 3000.0f));
}

void F16::update(float dt, World* world, KeyHandler& key_handler) {
    Aircraft::update(dt, world);

    engines[0].addThrottle((key_handler[SDL_SCANCODE_LSHIFT] - key_handler[SDL_SCANCODE_LCTRL]) * dt);

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