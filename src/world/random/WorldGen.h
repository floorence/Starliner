#pragma once
#include "world/Region.h"
#include <cstdint>
#include <sys/types.h>

class WorldGen {
public:
    WorldGen(uint seed);
    uint64_t splitmix64(uint64_t x);
    uint64_t hashRegion(Region region, uint64_t salt);
    double hash01(Region region, uint64_t salt); // [0, 1) 
    uint getRegionSeed(Region region);

private:
    uint seed;
};