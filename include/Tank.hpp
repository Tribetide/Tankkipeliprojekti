#ifndef TANK_HPP
#define TANK_HPP

#include <SFML/Graphics.hpp>
#include "Projectile.hpp"
#include "Terrain.hpp" 



class Tank {

public:
    Tank();
    // 🔥 Piirtäminen ja päivittäminen
    void draw(sf::RenderWindow &window);
    void update(Terrain &terrain, float gravity);


    // 🔥 Liikkuminen ja ohjaus
    void move(float dx, Terrain &terrain);
    void rotateTurret(float angleDelta);
    void adjustPower(float powerDelta);
    Projectile shoot();
    void placeOnTerrain(Terrain &terrain, int startX);
    void handleInput(sf::Keyboard::Key key, Terrain &terrain, 
                    std::vector<Projectile> &projectiles, bool &waitingForTurnSwitch, 
                    sf::Clock &turnClock);


    // 🔥 Tuhoutuminen ja elämäpisteet
    void takeDamage(int damage);
    void heal(int amount);
    int getHp() const;
    bool isDestroyed() const { return destroyed; }
    int getFuel() const;


    // 🔥 Getterit
    float getAngle() const;
    float getPower() const;
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const ; // Törmäyslaatikko

    void update();

    // 🔥 Reset (aloitustilanteeseen)
    void reset(Terrain& terrain, const sf::Vector2f& startPosition);
    void resetFuel();


private:   
    // 🔥 Muodot
    sf::Texture lowerBodyTexture, upperBodyTexture, turretTexture;
    sf::Sprite lowerBodySprite, upperBodySprite, turretSprite;
    sf::CircleShape upperBody;  
    sf::RectangleShape lowerBody;
    sf::RectangleShape turret;

    // 🔥 Attribuutit
    int hp;
    bool destroyed;
    float angle;
    float power;
    int fuel;

    // 🔥 Sijainti
    sf::Vector2f initialPosition; // Tallentaa alkuperäisen sijainnin
};

#endif
