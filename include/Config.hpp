#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <cstdlib>
#include <ctime>
#pragma once

namespace Config {

    // Fontin polut
    const std::string FONT_PATH_1 = "assets/fonts/arial.ttf";       // 🔥 Oletuspolku
    const std::string FONT_PATH_2 = "../assets/fonts/arial.ttf";    // 🔥 Jos ajetaan `build/`
    const std::string FONT_PATH_3 = "../../assets/fonts/arial.ttf"; // 🔥 Jos ajetaan `build/Debug/`

    // Painovoima
    static const float GRAVITY = 50.0f; // painovoima, px/s^2, käytetään tankkien ja ammuksien liikkeessä

    // Tuulen minimi- ja maksimiarvot
    static const int WIND_MIN = -15; 
    static const int WIND_MAX = 15;

    // Yksi apufunktio satunnaisen tuulen arvontaan
    inline float getRandomWind() {
        // Huom: varmista srand() vain kerran ohjelman alussa, 
        // tai tee staattinen tarkistus:
        // static bool seeded = false;
        // if (!seeded) { seeded = true; std::srand(std::time(nullptr)); }

        int range = WIND_MAX - WIND_MIN + 1; 
        int val = std::rand() % range + WIND_MIN;
        return static_cast<float>(val);
    }

    // Ammuksen nopeus ja lisänopeus
    static const float BASE_PROJECTILE_SPEED = 150.0f; 
    static const float MAX_EXTRA_PROJECTILE_SPEED = 300.0f; 
}

#endif
