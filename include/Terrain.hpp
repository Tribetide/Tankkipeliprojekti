#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <SFML/Graphics.hpp>

class Terrain {
public:
    Terrain();
    void initialize();
    void draw(sf::RenderWindow &window);
    bool checkCollision(sf::Vector2f position);
    void destroy(sf::Vector2f position, int radius);
    void createSky();

private:
    sf::Texture texture;
    sf::Image terrainImage;  // 🔥 Tämä lisätty! 
    sf::Sprite sprite;
    sf::CircleShape moon;
    std::vector<sf::Vector2f> stars; // ⭐ Tähtien sijainnit
};

#endif
