#include "GenRandom.h"

uint64_t GenRandom::splitmix64(uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;

    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    x = x ^ (x >> 31);

    return x;
}

uint64_t GenRandom::hashRegion(uint seed, Region region, uint64_t salt) {
    uint64_t h = seed;

    h = splitmix64(h ^ static_cast<uint64_t>(region.x));
    h = splitmix64(h ^ static_cast<uint64_t>(region.y));
    h = splitmix64(h ^ static_cast<uint64_t>(region.z));
    h = splitmix64(h ^ salt);

    return h;
}

double GenRandom::hash01(uint seed, Region region, uint64_t salt) {
    uint64_t h = hashRegion(seed, region, salt);

    return static_cast<double>(h) / static_cast<double>(UINT64_MAX);
}

uint GenRandom::getRegionSeed(uint seed, Region region) {
    return seed ^ (region.x * 37) ^ (region.y * 67) ^ (region.z * 73); // prime numbers
}

int GenRandom::randomInt(std::mt19937& gen, int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

float GenRandom::randomFloat(std::mt19937& gen, float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}