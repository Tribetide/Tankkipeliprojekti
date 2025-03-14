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
    void placeOnTerrain(Terrain &terrain, int startX);
    void update(Terrain &terrain, float gravity); //UUSI******
    void move(float dx, Terrain &terrain);
    sf::Vector2f getPosition() const; // Lisätään, jotta voidaan tarkistaa sijainti
    void handleInput(sf::Keyboard::Key key, Terrain &terrain, std::vector<Projectile> &projectiles, bool &waitingForTurnSwitch, sf::Clock &turnClock);

    


private:
    sf::CircleShape upperBody;  
    sf::RectangleShape lowerBody;
    sf::RectangleShape turret;
    float angle;
    float power;
    
};

#endif
