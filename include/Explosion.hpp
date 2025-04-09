// Explosion.hpp
#pragma once
#include <SFML/Graphics.hpp>

class Explosion {
public:
    Explosion(sf::Vector2f position);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    bool isFinished() const;

    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }

private:
    sf::CircleShape shape;
    float duration;
    float timePassed;
};