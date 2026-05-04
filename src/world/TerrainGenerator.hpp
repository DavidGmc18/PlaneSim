#pragma once

#include <FastNoiseLite/FastNoiseLite.h>

class TerrainGenerator {
    FastNoiseLite noise;
    float base;
    float amplitude;
    float frequency;

public:
    TerrainGenerator() = default;
    TerrainGenerator(float base, float amplitude, float frequency);
    float getHeight(float x, float z) const;
};