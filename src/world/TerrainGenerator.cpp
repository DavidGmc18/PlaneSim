#include "TerrainGenerator.hpp"

static FastNoiseLite noise;

TerrainGenerator::TerrainGenerator(float base, float amplitude, float frequency) {
    this->base = base;
    this->amplitude = amplitude;
    this->frequency = frequency;

    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(4);
    noise.SetFractalLacunarity(2.0f);
    noise.SetFractalGain(0.5f);
}

float TerrainGenerator::getHeight(float x, float z) const {
    return base + amplitude * noise.GetNoise(x * frequency, z * frequency);
}