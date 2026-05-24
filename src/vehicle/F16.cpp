#include "F16.hpp"
#include "Wing.hpp"
#include "Engine.hpp"
#include "Hitbox.hpp"

const Airfoil FUSELAGE(-40.0f, 40.0f, {
    /* -40° */ AirfoilSample(-0.380f, 0.265f, 0.000f),
    /* -35° */ AirfoilSample(-0.390f, 0.215f, 0.000f),
    /* -30° */ AirfoilSample(-0.400f, 0.172f, 0.000f),
    /* -25° */ AirfoilSample(-0.380f, 0.140f, 0.000f),
    /* -20° */ AirfoilSample(-0.340f, 0.123f, 0.000f),
    /* -15° */ AirfoilSample(-0.270f, 0.117f, 0.000f),
    /* -10° */ AirfoilSample(-0.180f, 0.113f, 0.000f),
    /*  -5° */ AirfoilSample(-0.080f, 0.111f, 0.000f),
    /*   0° */ AirfoilSample( 0.020f, 0.110f, 0.000f),
    /*   5° */ AirfoilSample( 0.120f, 0.111f, 0.000f),
    /*  10° */ AirfoilSample( 0.210f, 0.113f, 0.000f),
    /*  15° */ AirfoilSample( 0.280f, 0.117f, 0.000f),
    /*  20° */ AirfoilSample( 0.330f, 0.123f, 0.000f),
    /*  25° */ AirfoilSample( 0.360f, 0.140f, 0.000f),
    /*  30° */ AirfoilSample( 0.370f, 0.172f, 0.000f),
    /*  35° */ AirfoilSample( 0.375f, 0.215f, 0.000f),
    /*  40° */ AirfoilSample( 0.380f, 0.265f, 0.000f),
});

const Airfoil MAIN_WING(-20.0f, 30.0f, {
    /* -20° */ AirfoilSample(-0.960f, 0.085f,  0.108f),
    /* -18° */ AirfoilSample(-1.032f, 0.065f,  0.094f),
    /* -16° */ AirfoilSample(-1.116f, 0.050f,  0.079f),
    /* -14° */ AirfoilSample(-1.152f, 0.038f,  0.061f),
    /* -12° */ AirfoilSample(-1.128f, 0.028f,  0.040f),
    /* -10° */ AirfoilSample(-1.020f, 0.020f,  0.022f),
    /*  -8° */ AirfoilSample(-0.816f, 0.015f,  0.007f),
    /*  -6° */ AirfoilSample(-0.600f, 0.012f, -0.004f),
    /*  -4° */ AirfoilSample(-0.384f, 0.010f, -0.014f),
    /*  -2° */ AirfoilSample(-0.168f, 0.009f, -0.022f),
    /*   0° */ AirfoilSample( 0.048f, 0.009f, -0.025f),
    /*   2° */ AirfoilSample( 0.264f, 0.010f, -0.027f),
    /*   4° */ AirfoilSample( 0.468f, 0.012f, -0.029f),
    /*   6° */ AirfoilSample( 0.660f, 0.015f, -0.031f),
    /*   8° */ AirfoilSample( 0.840f, 0.020f, -0.032f),
    /*  10° */ AirfoilSample( 0.996f, 0.027f, -0.034f),
    /*  12° */ AirfoilSample( 1.116f, 0.037f, -0.036f),
    /*  14° */ AirfoilSample( 1.188f, 0.052f, -0.040f),
    /*  16° */ AirfoilSample( 1.212f, 0.072f, -0.047f),
    /*  18° */ AirfoilSample( 1.128f, 0.100f, -0.054f),
    /*  20° */ AirfoilSample( 0.984f, 0.138f, -0.068f),
    /*  22° */ AirfoilSample( 0.816f, 0.180f, -0.086f),
    /*  24° */ AirfoilSample( 0.660f, 0.228f, -0.104f),
    /*  26° */ AirfoilSample( 0.516f, 0.280f, -0.112f),
    /*  28° */ AirfoilSample( 0.396f, 0.335f, -0.108f),
    /*  30° */ AirfoilSample( 0.288f, 0.390f, -0.099f),
});

const Airfoil LERX(-20.0f, 40.0f, {
    /* -20° */ AirfoilSample(-0.684f, 0.072f,  0.043f),
    /* -18° */ AirfoilSample(-0.744f, 0.058f,  0.036f),
    /* -16° */ AirfoilSample(-0.756f, 0.046f,  0.029f),
    /* -14° */ AirfoilSample(-0.756f, 0.036f,  0.023f),
    /* -12° */ AirfoilSample(-0.744f, 0.026f,  0.018f),
    /* -10° */ AirfoilSample(-0.696f, 0.020f,  0.013f),
    /*  -8° */ AirfoilSample(-0.624f, 0.016f,  0.009f),
    /*  -6° */ AirfoilSample(-0.516f, 0.013f,  0.005f),
    /*  -4° */ AirfoilSample(-0.408f, 0.011f,  0.002f),
    /*  -2° */ AirfoilSample(-0.288f, 0.009f, -0.011f),
    /*   0° */ AirfoilSample( 0.048f, 0.009f, -0.025f),
    /*   2° */ AirfoilSample( 0.252f, 0.010f, -0.027f),
    /*   4° */ AirfoilSample( 0.468f, 0.012f, -0.029f),
    /*   6° */ AirfoilSample( 0.660f, 0.015f, -0.031f),
    /*   8° */ AirfoilSample( 0.852f, 0.021f, -0.032f),
    /*  10° */ AirfoilSample( 1.020f, 0.028f, -0.034f),
    /*  12° */ AirfoilSample( 1.152f, 0.038f, -0.036f),
    /*  14° */ AirfoilSample( 1.248f, 0.054f, -0.040f),
    /*  16° */ AirfoilSample( 1.344f, 0.075f, -0.045f),
    /*  18° */ AirfoilSample( 1.404f, 0.103f, -0.050f),
    /*  20° */ AirfoilSample( 1.380f, 0.142f, -0.056f),
    /*  22° */ AirfoilSample( 1.284f, 0.184f, -0.065f),
    /*  24° */ AirfoilSample( 1.152f, 0.232f, -0.076f),
    /*  26° */ AirfoilSample( 1.020f, 0.284f, -0.086f),
    /*  28° */ AirfoilSample( 0.852f, 0.340f, -0.094f),
    /*  30° */ AirfoilSample( 0.684f, 0.398f, -0.097f),
    /*  32° */ AirfoilSample( 0.516f, 0.452f, -0.095f),
    /*  34° */ AirfoilSample( 0.360f, 0.495f, -0.090f),
    /*  36° */ AirfoilSample( 0.228f, 0.528f, -0.081f),
    /*  38° */ AirfoilSample( 0.108f, 0.548f, -0.072f),
    /*  40° */ AirfoilSample( 0.024f, 0.558f, -0.061f),
});

const Airfoil HORIZONTAL_STAB(-25.0f, 25.0f, {
    /* -25° */ AirfoilSample(-0.58f, 0.085f, 0.0f),
    /* -20° */ AirfoilSample(-0.62f, 0.060f, 0.0f),
    /* -15° */ AirfoilSample(-0.68f, 0.040f, 0.0f),
    /* -10° */ AirfoilSample(-0.52f, 0.020f, 0.0f),
    /*  -5° */ AirfoilSample(-0.29f, 0.012f, 0.0f),
    /*   0° */ AirfoilSample( 0.06f, 0.010f, 0.0f),
    /*   5° */ AirfoilSample( 0.36f, 0.014f, 0.0f),
    /*  10° */ AirfoilSample( 0.58f, 0.024f, 0.0f),
    /*  15° */ AirfoilSample( 0.72f, 0.042f, 0.0f),
    /*  20° */ AirfoilSample( 0.68f, 0.072f, 0.0f),
    /*  25° */ AirfoilSample( 0.55f, 0.115f, 0.0f),
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
    inertia[1][2] = 1331.0f;
    inertia[2][1] = 1331.0f;
    inverse_inertia = glm::inverse(inertia);

    parts.push_back(new Engine(glm::vec3(0, 0, -4.5f), glm::vec3(0, 0, 1), 64900.0f, 1.0f, &this->fbw.throttle));

    parts.push_back(new Wing("Fuselage", &FUSELAGE, glm::vec3(0.0f, 0.0f, 0.3f), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), 15.0f, 4.5f));

    // sweep - 40°, +1° vertical
    parts.push_back(new Wing("Main wing L", &MAIN_WING, glm::vec3(-2.2f, 0.0f, 0.1f), glm::vec3(-0.643f, 0.013f, 0.766f), glm::vec3(0.0f, 0.999f, -0.017f), 4.959f, 2.8f));
    parts.push_back(new Wing("Main wing R", &MAIN_WING, glm::vec3( 2.2f, 0.0f, 0.1f), glm::vec3( 0.643f, 0.013f, 0.766f), glm::vec3(0.0f, 0.999f, -0.017f), 4.959f, 2.8f));

    // sweep - 40°, -2° vertical
    parts.push_back(new Wing("Wing tip L", &MAIN_WING, glm::vec3(-4.1f, 0.0f, -0.2f), glm::vec3(-0.643f, -0.027f, 0.766f), glm::vec3(0.0f, 0.999f, 0.035f), 3.306f, 1.6f, &this->fbw.flaperon_l, -10, 10, 80));
    parts.push_back(new Wing("Wing tip R", &MAIN_WING, glm::vec3( 4.1f, 0.0f, -0.2f), glm::vec3( 0.643f, -0.027f, 0.766f), glm::vec3(0.0f, 0.999f, 0.035f), 3.306f, 1.6f, &this->fbw.flaperon_r, -10, 10, 80));
    
    parts.push_back(new Wing("LERX L", &LERX, glm::vec3(-1.0f, 0.0f, 0.1f), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), 6.235f, 3.8f));
    parts.push_back(new Wing("LERX R", &LERX, glm::vec3( 1.0f, 0.0f, 0.1f), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), 6.235f, 3.8f));

    // Anhedral - 10°
    parts.push_back(new Wing("Horizontal stabilizer L", &HORIZONTAL_STAB, glm::vec3(-2.5f, 0.0f, -7.0f), glm::vec3(0, 0, 1), glm::vec3(-0.174f, 0.985f, 0), 5.9f, 1.9f, &this->fbw.taileron_l, -25, 21, 60));
    parts.push_back(new Wing("Horizontal stabilizer R", &HORIZONTAL_STAB, glm::vec3( 2.5f, 0.0f, -7.0f), glm::vec3(0, 0, 1), glm::vec3( 0.174f, 0.985f, 0), 5.9f, 1.9f, &this->fbw.taileron_r, -25, 21, 60));

    parts.push_back(new Wing("Vertical stabilizer", &VERTICAL_STAB, glm::vec3(0.0f, 1.5f, -6.8f), glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), 6.0f, 2.4f, &this->fbw.rudder, -10, 10, 120));

    parts.push_back(new Hitbox(glm::vec3( 0.0f, -0.835f,  2.4f), 0.110f, 500000.0f, 1000.0f));
    parts.push_back(new Hitbox(glm::vec3( 1.0f, -0.780f, -1.0f), 0.165f, 500000.0f, 3000.0f));
    parts.push_back(new Hitbox(glm::vec3(-1.0f, -0.780f, -1.0f), 0.165f, 500000.0f, 3000.0f));
}

void F16::update(World* world, float dt, const AircraftControls* controls) {
    fbw.taileron_l = 25.0f * controls->getAxisValue(AircraftControls::PITCH);
    fbw.taileron_r = 25.0f * controls->getAxisValue(AircraftControls::PITCH);

    fbw.flaperon_l = -10.0f * controls->getAxisValue(AircraftControls::ROLL);
    fbw.flaperon_r = 10.0f * controls->getAxisValue(AircraftControls::ROLL);

    fbw.rudder = 10.0f * controls->getAxisValue(AircraftControls::YAW);

    fbw.throttle = controls->getAxisValue(AircraftControls::THROTTLE);

    RigidBody::update(world, dt);

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