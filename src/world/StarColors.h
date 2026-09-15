#pragma once

/**
 * each star gets a random mass which is used to calculate its temperature and luminosity.
 * in order to have the colours have more of an equal chance of showing, relation of mass to temperature is linear
 * even though that's not the case in real life, and temperature in kilo-Kelvin is within [2, 18], even though stars
 * can be much hotter in real life.
 */
namespace StarColors {
    /**
     * r = 100            [2, 10)
     * r = 212.5 - 11.25T [10, 18]
     */
    inline float r(float temp) {
        if (temp < 10) {
            return 100.0f;
        } else {
            return 212.5f - 11.25f * temp;
        }
    }

    /**
     * g = -35 + 22.5T    [2, 6)
     * g = 100            [6, 10)
     * g = 212.5 - 11.25T [10, 18]
     */
    inline float g(float temp) {
        if (temp < 6) {
            return -35.0f + 22.5 * temp;
        } else if (temp < 10) {
            return 100.0f;
        } else {
            return 212.5f - 11.25 * temp;
        }
    }

    /**
     * b = -12.5 + 11.25T [2, 10)
     * b = 100            [10, 18]
     */
    inline float b(float temp) {
        if (temp < 10) {
            return -12.5f + 11.25f * temp;
        } else {
            return 100.0f;
        }
    }
}