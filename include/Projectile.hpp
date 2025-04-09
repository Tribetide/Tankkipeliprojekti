#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <SFML/Graphics.hpp>
#include "Terrain.hpp"

class Projectile {
public:
    Projectile();
    
    // 🔥 Ammuksen muodot
    sf::CircleShape shape;

    // 🔥 Ammuksen liike
    sf::Vector2f velocity;
    bool alive;

    // 🔥 Päivitys ja piirtäminen
    //void update(float gravity, Terrain &terrain, float windForce);
    void update(float deltaTime, Terrain &terrain, float windForce);
    void draw(sf::RenderWindow &window) const;

    // 🔥 Törmäystarkistus
    sf::FloatRect getBounds() const; 

    // 🔥 Painovoiman asetus
    void setGravity(float gravity);
 


    
private:
    float gravity;
};

#endif