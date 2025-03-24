#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <SFML/Graphics.hpp>
#include <vector>

class Terrain {
public:
    Terrain();
    void initialize();                               // Generoi maasto
    void draw(sf::RenderWindow &window);             // Piirtää maaston
    bool checkCollision(sf::Vector2f position);      // Törmäystarkistus
    void destroy(sf::Vector2f position, int radius); // Tuhotaan pikseleitä
    void createSky();                                // Luo tähdet
    void update(float deltaTime);                    // Päivittää tähdenlennot

private:
    sf::Texture texture;
    sf::Image terrainImage;
    sf::Sprite sprite;

    // Tähtiefektejä varten
    sf::CircleShape moon; // (jos haluat pitää kuun)
    std::vector<sf::Vector2f> stars;

    struct ShootingStar {
        sf::Vector2f position;
        sf::Vector2f velocity;
        float lifetime;
        std::vector<sf::Vector2f> previousPositions;
    };

    std::vector<ShootingStar> shootingStars;
    float shootingStarTimer = 50.0f;
};

#endif
