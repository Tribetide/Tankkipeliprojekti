// Explosion.hpp
#pragma once
#include <SFML/Graphics.hpp>

class Explosion {
public:
    Explosion(sf::Vector2f position);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    bool isFinished() const;

private:
    sf::CircleShape shape;
    float duration;
    float timePassed;
};