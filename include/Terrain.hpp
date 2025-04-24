#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <SFML/Graphics.hpp>
#include <vector>

struct PixelInfo {
    sf::Vector2i coords;
    sf::Color color;
};

class Terrain {
public:
    Terrain();
    void initialize();                               // Generoi maasto
    void draw(sf::RenderWindow &window);             // Piirtää maaston
    bool checkCollision(const sf::Vector2f& point) const;  // Törmäystarkistus
    std::vector<PixelInfo> destroy(sf::Vector2f position, int baseRadius); // Tuhotaan pikseleitä
    void createSky();                                // Luo tähdet
    void update(float deltaTime);                    // Päivittää tähdenlennot
    bool isLavaAt(sf::Vector2f point) const;

private:
    sf::Texture texture;
    sf::Image terrainImage;
    sf::Sprite sprite;

    // Tähtiefektejä varten
    sf::CircleShape moon; // (jos haluat pitää kuun)
    std::vector<sf::Vector2f> stars;
    
    bool isLavaPixel(const sf::Color& c) const; 

    struct ShootingStar {
        sf::Vector2f position;
        sf::Vector2f velocity;
        float lifetime;
        std::vector<sf::Vector2f> previousPositions;
    };

    std::vector<ShootingStar> shootingStars;
    float shootingStarTimer = 50.0f;
};

struct Debris {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float lifetime;

    Debris(sf::Vector2f pos, sf::Vector2f vel, sf::Color c, float life)
        : position(pos), velocity(vel), color(c), lifetime(life) {}

};

#endif
