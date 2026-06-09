#include "TerrainGenerator.hpp"
#include <glm/glm.hpp>

TerrainGenerator::TerrainGenerator(int seed) {
    this->plain_noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    this->plain_noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    this->plain_noise.SetSeed(seed);
    this->plain_noise.SetFractalOctaves(3);

    this->mountain_noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    this->mountain_noise.SetFractalType(FastNoiseLite::FractalType_Ridged);
    this->mountain_noise.SetSeed(seed + 67);
    this->mountain_noise.SetFractalOctaves(4);
}

const float RNW_WIDTH = 32.0f;
const float RNW_LENGTH = 2400.0f;
const float RNW_RADIUS = 64.0f;

const float VALLEY_RADIUS = 1500.0f;

float TerrainGenerator::getHeight(float x, float z) const {
    if (x >= -(RNW_WIDTH/2) && x <= (RNW_WIDTH/2) && z >= -(RNW_LENGTH/2) && z <= (RNW_LENGTH/2)) {
        return 0.0f;
    }

    const float freq = 0.02f;
    float plain_height = 50.0f * this->plain_noise.GetNoise(x * freq, z * freq);
    float mountain_height = 500.0f * this->mountain_noise.GetNoise(x * freq, z * freq);

    float distance_valley = glm::distance(glm::vec2(x, z), glm::vec2(0.0f));
    float distance_runway = glm::distance(glm::vec2(x, z), glm::vec2(0.0f, glm::clamp(z, -(RNW_LENGTH/2), (RNW_LENGTH/2))));

    if (distance_valley < VALLEY_RADIUS) {
        float r = glm::pow(distance_valley / VALLEY_RADIUS, 10);
        float height = (1.0f - r) * plain_height + r * mountain_height;
        
        if (distance_runway < RNW_RADIUS ) {
            height *= distance_runway / RNW_RADIUS;
        }

        return height;
    }

    return mountain_height;
}