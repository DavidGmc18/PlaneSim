#pragma once

#include <FastNoiseLite/FastNoiseLite.h>

class TerrainGenerator {
    FastNoiseLite plain_noise;
    FastNoiseLite mountain_noise;

public:
    TerrainGenerator(int seed);
    float getHeight(float x, float z) const;
};