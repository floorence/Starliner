#pragma once

#include "world/Region.h"
#include <cstdint>
#include <random>
#include <sys/types.h>

class GenRandom {
public:
    static uint64_t splitmix64(uint64_t x);
    static uint64_t hashRegion(uint seed, Region region, uint64_t salt);
    static double hash01(uint seed, Region region, uint64_t salt); // [0, 1) 

    static uint getRegionSeed(uint seed, Region region);
    // [min, max]
    static int randomInt(std::mt19937& gen, int min, int max);
    static float randomFloat(std::mt19937& gen, float min, float max);
};