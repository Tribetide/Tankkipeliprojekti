#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>

#include "Tank.hpp"
#include "Terrain.hpp"
#include "Projectile.hpp"

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update();
    void render();

    sf::RenderWindow window;
    Tank tank;
    Terrain terrain;
    std::vector<Projectile> projectiles;
    float gravity; // painovoima
    float windForce; // tuuli

    sf::Font font; // Lisätään globaali fontti
};

#endif
