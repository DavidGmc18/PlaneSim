#include "F16.hpp"

const Airfoil FUSELAGE(-40.0f, 40.0f, {
    /* -40° */ AirfoilSample(-0.380f, 0.320f, 0.000f),
    /* -35° */ AirfoilSample(-0.390f, 0.255f, 0.000f),
    /* -30° */ AirfoilSample(-0.400f, 0.200f, 0.000f),
    /* -25° */ AirfoilSample(-0.400f, 0.145f, 0.000f),
    /* -20° */ AirfoilSample(-0.400f, 0.100f, 0.000f),
    /* -15° */ AirfoilSample(-0.320f, 0.055f, 0.000f),
    /* -10° */ AirfoilSample(-0.220f, 0.034f, 0.000f),
    /*  -5° */ AirfoilSample(-0.100f, 0.018f, 0.000f),
    /*   0° */ AirfoilSample( 0.020f, 0.012f, 0.000f),
    /*   5° */ AirfoilSample( 0.140f, 0.018f, 0.000f),
    /*  10° */ AirfoilSample( 0.240f, 0.034f, 0.000f),
    /*  15° */ AirfoilSample( 0.320f, 0.055f, 0.000f),
    /*  20° */ AirfoilSample( 0.360f, 0.100f, 0.000f),
    /*  25° */ AirfoilSample( 0.370f, 0.145f, 0.000f),
    /*  30° */ AirfoilSample( 0.380f, 0.200f, 0.000f),
    /*  35° */ AirfoilSample( 0.385f, 0.255f, 0.000f),
    /*  40° */ AirfoilSample( 0.390f, 0.320f, 0.000f),
});

const Airfoil MAIN_WING(-20.0f, 30.0f, {
    /* -20° */ AirfoilSample(-0.960f, 0.460f,  0.060f),
    /* -18° */ AirfoilSample(-1.032f, 0.380f,  0.052f),
    /* -16° */ AirfoilSample(-1.116f, 0.310f,  0.044f),
    /* -14° */ AirfoilSample(-1.152f, 0.245f,  0.034f),
    /* -12° */ AirfoilSample(-1.128f, 0.185f,  0.022f),
    /* -10° */ AirfoilSample(-1.020f, 0.130f,  0.012f),
    /*  -8° */ AirfoilSample(-0.816f, 0.085f,  0.004f),
    /*  -6° */ AirfoilSample(-0.600f, 0.050f, -0.002f),
    /*  -4° */ AirfoilSample(-0.384f, 0.026f, -0.008f),
    /*  -2° */ AirfoilSample(-0.168f, 0.014f, -0.012f),
    /*   0° */ AirfoilSample( 0.048f, 0.009f, -0.014f),
    /*   2° */ AirfoilSample( 0.264f, 0.014f, -0.015f),
    /*   4° */ AirfoilSample( 0.468f, 0.026f, -0.016f),
    /*   6° */ AirfoilSample( 0.660f, 0.050f, -0.017f),
    /*   8° */ AirfoilSample( 0.840f, 0.085f, -0.018f),
    /*  10° */ AirfoilSample( 0.996f, 0.130f, -0.019f),
    /*  12° */ AirfoilSample( 1.116f, 0.185f, -0.020f),
    /*  14° */ AirfoilSample( 1.188f, 0.245f, -0.022f),
    /*  16° */ AirfoilSample( 1.212f, 0.310f, -0.026f),
    /*  18° */ AirfoilSample( 1.128f, 0.380f, -0.030f),
    /*  20° */ AirfoilSample( 0.984f, 0.460f, -0.038f),
    /*  22° */ AirfoilSample( 0.816f, 0.550f, -0.048f),
    /*  24° */ AirfoilSample( 0.660f, 0.650f, -0.058f),
    /*  26° */ AirfoilSample( 0.516f, 0.760f, -0.062f),
    /*  28° */ AirfoilSample( 0.396f, 0.880f, -0.060f),
    /*  30° */ AirfoilSample( 0.288f, 1.010f, -0.055f),
});

const Airfoil LERX(-20.0f, 40.0f, {
    /* -20° */ AirfoilSample(-0.684f, 0.520f,  0.024f),
    /* -18° */ AirfoilSample(-0.744f, 0.430f,  0.020f),
    /* -16° */ AirfoilSample(-0.756f, 0.350f,  0.016f),
    /* -14° */ AirfoilSample(-0.756f, 0.280f,  0.013f),
    /* -12° */ AirfoilSample(-0.744f, 0.210f,  0.010f),
    /* -10° */ AirfoilSample(-0.696f, 0.145f,  0.007f),
    /*  -8° */ AirfoilSample(-0.624f, 0.095f,  0.005f),
    /*  -6° */ AirfoilSample(-0.516f, 0.055f,  0.003f),
    /*  -4° */ AirfoilSample(-0.408f, 0.028f,  0.001f),
    /*  -2° */ AirfoilSample(-0.288f, 0.015f, -0.006f),
    /*   0° */ AirfoilSample( 0.048f, 0.009f, -0.014f),
    /*   2° */ AirfoilSample( 0.252f, 0.015f, -0.015f),
    /*   4° */ AirfoilSample( 0.468f, 0.028f, -0.016f),
    /*   6° */ AirfoilSample( 0.660f, 0.055f, -0.017f),
    /*   8° */ AirfoilSample( 0.852f, 0.095f, -0.018f),
    /*  10° */ AirfoilSample( 1.020f, 0.145f, -0.019f),
    /*  12° */ AirfoilSample( 1.152f, 0.210f, -0.020f),
    /*  14° */ AirfoilSample( 1.248f, 0.280f, -0.022f),
    /*  16° */ AirfoilSample( 1.344f, 0.350f, -0.025f),
    /*  18° */ AirfoilSample( 1.404f, 0.430f, -0.028f),
    /*  20° */ AirfoilSample( 1.380f, 0.520f, -0.031f), 
    /*  22° */ AirfoilSample( 1.284f, 0.620f, -0.036f),
    /*  24° */ AirfoilSample( 1.152f, 0.730f, -0.042f),
    /*  26° */ AirfoilSample( 1.020f, 0.850f, -0.048f),
    /*  28° */ AirfoilSample( 0.852f, 0.980f, -0.052f),
    /*  30° */ AirfoilSample( 0.684f, 1.120f, -0.054f),
    /*  32° */ AirfoilSample( 0.516f, 1.270f, -0.053f),
    /*  34° */ AirfoilSample( 0.360f, 1.430f, -0.050f),
    /*  36° */ AirfoilSample( 0.228f, 1.600f, -0.045f),
    /*  38° */ AirfoilSample( 0.108f, 1.780f, -0.040f),
    /*  40° */ AirfoilSample( 0.024f, 1.950f, -0.034f),
});

const Airfoil HORIZONTAL_STAB(-25.0f, 25.0f, {
    /* -25° */ AirfoilSample(-0.90f, 0.085f, 0.0f),
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
    gravity = true;

    glm::mat3 inertia = glm::mat3(0.0f);
    inertia[0][0] = 75700.0f;
    inertia[1][1] = 85600.0f;
    inertia[2][2] = 12875.0f;
    inertia[2][1] = 1331.0f;
    inverse_inertia = glm::inverse(inertia);

    parts.push_back(new Engine(glm::vec3(0, 0, -4.5f), glm::vec3(0, 0, 1), 64900.0f, 1.0f, this->controls[THROTTLE].get()));

    parts.push_back(new Wing("Fuselage", &FUSELAGE, glm::vec3(0.0f, 0.0f, 0.4f), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), 15.0f, 4.5f, NULL, 0, 0));

    // sweep - 40°, +1° vertical
    parts.push_back(new Wing("Main wing L", &MAIN_WING, glm::vec3(-2.2f, 0.0f, 0.4f), glm::vec3(-0.643f, 0.013f, 0.766f), glm::vec3(0.0f, 0.999f, -0.017f), 4.959f, 2.8f, NULL, 0, 0));
    parts.push_back(new Wing("Main wing R", &MAIN_WING, glm::vec3( 2.2f, 0.0f, 0.4f), glm::vec3( 0.643f, 0.013f, 0.766f), glm::vec3(0.0f, 0.999f, -0.017f), 4.959f, 2.8f, NULL, 0, 0));

    // sweep - 40°, -2° vertical
    parts.push_back(new Wing("Wing tip L", &MAIN_WING, glm::vec3(-4.1f, 0.0f, -0.2f), glm::vec3(-0.643f, -0.027f, 0.766f), glm::vec3(0.0f, 0.999f, 0.035f), 3.306f, 1.6f, this->controls[ROLL].get(), 10, -10));
    parts.push_back(new Wing("Wing tip R", &MAIN_WING, glm::vec3( 4.1f, 0.0f, -0.2f), glm::vec3( 0.643f, -0.027f, 0.766f), glm::vec3(0.0f, 0.999f, 0.035f), 3.306f, 1.6f, this->controls[ROLL].get(), -10, 10));
    
    parts.push_back(new Wing("LERX L", &LERX, glm::vec3(-1.0f, 0.0f, -0.2f), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), 6.235f, 3.8f, NULL, 0, 0));
    parts.push_back(new Wing("LERX R", &LERX, glm::vec3( 1.0f, 0.0f, -0.2f), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), 6.235f, 3.8f, NULL, 0, 0));

    float anhedral = glm::radians(10.0f);
    parts.push_back(new Wing("Horizontal stabilizer L", &HORIZONTAL_STAB, glm::vec3(-2.5f, 0.0f, -7.0f), glm::vec3(0, 0, 1), glm::vec3(-glm::sin(anhedral), glm::cos(anhedral), 0), 5.9f, 1.9f, this->controls[PITCH].get(), -25, 21));
    parts.push_back(new Wing("Horizontal stabilizer R", &HORIZONTAL_STAB, glm::vec3( 2.5f, 0.0f, -7.0f), glm::vec3(0, 0, 1), glm::vec3( glm::sin(anhedral), glm::cos(anhedral), 0), 5.9f, 1.9f, this->controls[PITCH].get(), -25, 21));

    parts.push_back(new Wing("Vertical stabilizer", &VERTICAL_STAB, glm::vec3(0.0f, 1.5f, -6.8f), glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), 6.0f, 2.4f, this->controls[YAW].get(), -10, 10));

    parts.push_back(new Hitbox(glm::vec3( 0.0f, -0.835f,  2.4f), 0.110f, 500000.0f, 1000.0f));
    parts.push_back(new Hitbox(glm::vec3( 1.0f, -0.780f, -1.0f), 0.165f, 500000.0f, 3000.0f));
    parts.push_back(new Hitbox(glm::vec3(-1.0f, -0.780f, -1.0f), 0.165f, 500000.0f, 3000.0f));
}

void F16::update(World* world, float dt, KeyHandler& key_handler) {
    Aircraft::update(world, dt);

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