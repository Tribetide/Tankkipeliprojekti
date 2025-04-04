#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "Tank.hpp"
#include "Terrain.hpp"
#include "Projectile.hpp"
#include "EventManager.hpp"  // Tämä pitää olla ennen Game-luokkaa
#include <memory>  // Tarvitaan std::unique_ptr
#include "Explosion.hpp"

class EventManager;


class Game {
public:
    Game();
    void run();
    void endGame();  // 🔥 Lisätty julkiseksi
    void resetGame();  // Lisää tämä metodi

private:
    void processEvents();
    void update();
    void render();
    void drawWindIndicator();

    std::vector<Explosion> explosions;
    sf::RenderWindow window;
    Tank tank1;
    Tank tank2;
    bool isPlayerOneTurn; // Vuorottelua
    sf::Texture moonTexture;
    sf::Sprite moonSprite;


    Terrain terrain;
    std::vector<Projectile> projectiles;
    float gravity; // 🔥  painovoima
    float windForce; // 🔥  tuuli

    sf::Font font; // 🔥 Lisätään globaali fontti
    EventManager eventManager;  // Tässä käytetään EventManageria

    sf::Clock turnClock;  // 🔥 Ajastin vuoron vaihtoa varten
    bool waitingForTurnSwitch = false;  // 🔥 Indikaattori, odotetaanko vuoron vaihtoa

    sf::Vector2f tank1StartPosition;
    sf::Vector2f tank2StartPosition;


};

#endif
