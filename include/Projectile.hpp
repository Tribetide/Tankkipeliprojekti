#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <SFML/Graphics.hpp>
#include "Terrain.hpp"

class Projectile {
public:
    Projectile();
    void update(float gravity, Terrain &terrain, float windForce);
    void setGravity(float gravity);  // 🔥 Mahdollistaa yksittäisen painovoiman asetuksen

    bool alive;
    sf::CircleShape shape;
    sf::Vector2f velocity;
    
private:
    float gravityEffect = 0.0005f;
};


#endif
