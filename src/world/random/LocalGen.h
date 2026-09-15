#pragma once

#include <random>
#include <sys/types.h>

class LocalGen {
public:
    LocalGen(uint localSeed): gen(localSeed) {}

    int randomInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(gen);
    }

    float randomFloat(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(gen);
    }

private:
    std::mt19937 gen;
};