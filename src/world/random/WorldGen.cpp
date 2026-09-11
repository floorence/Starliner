#include "WorldGen.h"

WorldGen::WorldGen(uint seed): seed(seed) {}

uint64_t WorldGen::splitmix64(uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;

    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    x = x ^ (x >> 31);

    return x;
}

uint64_t WorldGen::hashRegion(Region region, uint64_t salt) {
    uint64_t h = seed;

    h = splitmix64(h ^ static_cast<uint64_t>(region.x));
    h = splitmix64(h ^ static_cast<uint64_t>(region.y));
    h = splitmix64(h ^ static_cast<uint64_t>(region.z));
    h = splitmix64(h ^ salt);

    return h;
}

double WorldGen::hash01(Region region, uint64_t salt) {
    uint64_t h = hashRegion(region, salt);

    return static_cast<double>(h) / static_cast<double>(UINT64_MAX);
}

uint WorldGen::getRegionSeed(Region region) {
    return seed ^ (region.x * 37) ^ (region.y * 67) ^ (region.z * 73); // prime numbers
}