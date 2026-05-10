#include "Aircraft.hpp"

void Aircraft::update(float dt, World* world) {
    for (Engine& engine : engines) {
        engine.apply_force(this);
    }

    for (Wing& wing : wings) {
        wing.apply_forces(this);
    }

    for (Hitbox& hitbox: hitboxes) {
        hitbox.apply_forces(this, world);
    }

    RigidBody::update(dt);
}