#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "Tank.hpp"
#include "Terrain.hpp"
#include "Projectile.hpp"
#include "EventManager.hpp"
#include "Explosion.hpp"
#include <memory>  // std::unique_ptr ja muut

// Etujulistus
class EventManager;

class Game {
public:
    Game();
    explicit Game(sf::RenderWindow& win);
    void run();                         // Käynnistää pelin pääsilmukan
    void endGame();                     // Päättää pelin ja näyttää tulokset
    void resetGame();                   // Nollaa peli uutta peliä varten
    void spawnDebris(const std::vector<PixelInfo>& destroyedPixels, sf::Vector2f center); // Partikkelien generointi

private:
    void processEvents();               // Käsittelee näppäimistö ja hiiri
    void update();                      // Pelilogiikan päivitys
    void render();                      // Piirto- ja UI-logiikka
    void drawWindIndicator();           // Tuulen graafinen esitys (jos käytössä)

    // Ikkuna ja visuaalinen tila
    sf::RenderWindow& window;
    sf::Texture moonTexture;
    sf::Sprite moonSprite;
    sf::Font font;

    // Pelin objektit
    Terrain terrain;
    Tank tank1;
    Tank tank2;
    std::vector<Projectile> projectiles;
    std::vector<Explosion> explosions;
    std::vector<Debris> debrisList;

    // Pelimekaniikka
    float gravity;
    float windForce;

    EventManager eventManager;
    bool waitingForTurnSwitch = false;
    sf::Clock turnClock;
    sf::Clock globalClock;

    // Tankkien aloitussijainnit
    sf::Vector2f tank1StartPosition;
    sf::Vector2f tank2StartPosition;

    // Score-järjestelmä
    int tank1Wins = 0;
    int tank2Wins = 0;
    int draws     = 0;
};

#endif
