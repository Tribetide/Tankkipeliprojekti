#ifndef TANK_HPP
#define TANK_HPP

#include <SFML/Graphics.hpp>
#include "Projectile.hpp"




class Tank {
public:
    Tank();
    void draw(sf::RenderWindow &window);
    void rotateTurret(float angleDelta);
    void adjustPower(float powerDelta);
    float getAngle() const;
    float getPower() const;
    Projectile shoot();
    void placeOnTerrain(Terrain &terrain); //UUSI
    void move(float dx);
    sf::Vector2f getPosition() const; // Lisätään, jotta voidaan tarkistaa sijainti
    


private:
    sf::CircleShape upperBody;  
    sf::RectangleShape lowerBody;
    sf::RectangleShape turret;
    float angle;
    float power;
    
};

#endif
