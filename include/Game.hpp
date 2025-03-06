#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "Tank.hpp"
#include "Terrain.hpp"
#include "Projectile.hpp"
#include "EventManager.hpp"

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update();
    void render();
    void drawWindIndicator();


    sf::RenderWindow window;
    Tank tank1;
    Tank tank2;
    bool isPlayerOneTurn; // Vuorottelua
    Terrain terrain;
    std::vector<Projectile> projectiles;
    float gravity; // 🔥  painovoima
    float windForce; // 🔥  tuuli

    sf::Font font; // 🔥 Lisätään globaali fontti
    EventManager eventManager;


    sf::Clock turnClock;  // 🔥 Ajastin vuoron vaihtoa varten
    bool waitingForTurnSwitch = false;  // 🔥 Indikaattori, odotetaanko vuoron vaihtoa

};

#endif