#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <cstdlib>
#include <ctime>
#pragma once

namespace Config {
    const std::string FONT_PATH_1 = "assets/fonts/SHOWG.TTF";       // 🔥 Oletuspolku
    const std::string FONT_PATH_2 = "../assets/fonts/SHOWG.TTF";    // 🔥 Jos ajetaan `build/`
    const std::string FONT_PATH_3 = "../../assets/fonts/SHOWG.TTF"; // 🔥 Jos ajetaan `build/Debug/`

    // Päivityssnopeus
    constexpr float FIXED_DT = 0.9f / 60.0f; //

    //Ruudun/kentän mitat
    static const int SCREEN_WIDTH = 1920; // Kentän leveys
    static const int SCREEN_HEIGHT = 1080; // Kentän korkeus
    static const int PROJECTILE_BOUND_LEFT = -50; // Ammuksen vasen raja
    static const int PROJECTILE_BOUND_RIGHT = 2000; // Ammuksen oikea raja
    static const int PROJECTILE_BOUND_TOP = -2000; // Ammuksen yläraja
    static const int PROJECTILE_BOUND_BOTTOM = 1200; // Ammuksen alaraja

    // Tankin ja tykin ohjaus
    static const float TANK_MOVE = 5.0f; // Tankin liikeaskel
    static const float TURRET_ROTATE = 5.0f; // Tykin kulman säätöaskel
    static const float POWER_MIN = 10.0f; // Ammuksen voiman minimiraja
    static const float POWER_MAX = 100.0f; // Ammuksen voiman maksimiarvo
    
    // Painovoima
    static const float GRAVITY = 50.0f; // painovoima, px/s^2, käytetään tankkien ja ammuksien liikkeessä

    // Tuulen minimi- ja maksimiarvot
    static const int WIND_MIN = -15; 
    static const int WIND_MAX = 15;

    // Yksi apufunktio satunnaisen tuulen arvontaan
    inline float getRandomWind() {

        int range = WIND_MAX - WIND_MIN + 1; 
        int val = std::rand() % range + WIND_MIN;
        return static_cast<float>(val);
    }

    //Ammuksen koko ja räjähdyksen säde
    static const float PROJECTILE_RADIUS = 5.f; // Ammuksen säde
    static const float EXPLOSION_RADIUS = 50.f; // Räjähdyksen säde
    static const float DIRECT_HIT_DAMAGE = 30.f; // Räjähdyksen vahinko

    //Räjähdys efektin koko, kesto, haalistuma
    static const float EXPLOSION_START_RADIUS = 10.f; // Räjähdyksen efektin koko
    static const float EXPLOSION_END_RADIUS = 120.f; // Räjähdyksen efektin loppukoko
    static const float EXPLOSION_DURATION = 2.0f; // Räjähdyksen kesto
    static const float EXPLOSION_FADE_RATIO = 0.7f; // Räjähdyksen haalistumisen kesto



    // Ammuksen nopeus ja lisänopeus
    static const float BASE_PROJECTILE_SPEED = 150.0f; 
    static const float MAX_EXTRA_PROJECTILE_SPEED = 225.0f; 

    // Tähtäimen koko
    static const float CROSSHAIR_SIZE = 20.f; // Tähtäimen koko

    // Aikarajat vuorolle, vaihtoon sekä pelin loppumiseen
    const float TURN_TIME_LIMIT = 30.0f;
    const float TURN_SWITCH_DELAY = 2.0f;
    const float ENDGAME_DELAY = 2.0f;
}

#endif
