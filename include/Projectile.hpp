#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <SFML/Graphics.hpp>
#include "Terrain.hpp"

class Projectile {
public:
    Projectile();
    void update(float deltaTime, Terrain &terrain);

    sf::CircleShape shape;
    sf::Vector2f velocity;
    bool alive;
    float gravityEffect; // 🔥 Voiman vaikutus ammuksen lentoon
};


#endif
