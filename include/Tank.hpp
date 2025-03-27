#ifndef TANK_HPP
#define TANK_HPP

#include <SFML/Graphics.hpp>
#include "Projectile.hpp"
#include "Terrain.hpp"  // Jos tarvitset Terrain-luokkaa



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

    // Lisää getterit ja setterit hp:lle
    int getHp() const;  // Määrittely ilman toteutusta
    void setHp(int newHp);  // Määrittely ilman toteutusta

    // Reset-metodi päivitetty hyväksymään aloituspaikan
    void reset(Terrain& terrain, const sf::Vector2f& startPosition);

private:

    sf::Texture lowerBodyTexture, upperBodyTexture, turretTexture;
    sf::Sprite lowerBodySprite, upperBodySprite, turretSprite;
    sf::RectangleShape upperBody;  
    sf::RectangleShape lowerBody;
    sf::RectangleShape turret;
    float angle;
    float power;
    int hp; // Lisätty hp (elämäpisteet)

    sf::Vector2f initialPosition; // Tallentaa alkuperäisen sijainnin
    
};

#endif
