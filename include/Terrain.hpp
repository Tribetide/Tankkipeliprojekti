#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <SFML/Graphics.hpp>
#include <vector>

class Terrain {
public:
    Terrain();
    void initialize();
    void draw(sf::RenderWindow &window);
    bool checkCollision(sf::Vector2f position);
    void destroy(sf::Vector2f position, int radius);
    void createSky();
    void update(float deltaTime);
    
private:
    sf::Texture texture;
    sf::Image terrainImage;  // 🔥 Tämä lisätty! 
    sf::Sprite sprite;
    sf::CircleShape moon;
    std::vector<sf::Vector2f> stars; // ⭐ Tähtien sijainnit

    struct ShootingStar { // ⭐ Tähdenlento
        sf::Vector2f position;
        sf::Vector2f velocity;
        float lifetime;
        std::vector<sf::Vector2f> previousPositions; // 🔥 Häntätehosteelle!
    };

    std::vector<ShootingStar> shootingStars; // ⭐ Tähtienlentoefektit
    float shootingStarTimer = 50.0f; // ⭐ Ajastin tähtienlennolle
};

#endif