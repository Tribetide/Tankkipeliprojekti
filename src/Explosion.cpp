// Explosion.cpp
#include "Explosion.hpp"
#include "Config.hpp"
#include <iostream>

Explosion::Explosion(sf::Vector2f position)
    : duration(Config::EXPLOSION_DURATION), timePassed(0.0f)
{
    shape.setRadius(Config::EXPLOSION_START_RADIUS); // Alkuarvo isommaksi
    shape.setOrigin(10.f, 10.f); // Asetetaan alkuperäinen keskipiste
    shape.setFillColor(sf::Color::Red); // Kirkkaampi
    shape.setPosition(position);
}

void Explosion::update(float deltaTime) {
    timePassed += deltaTime;

    float progress = timePassed / duration;
    shape.setRadius(Config::EXPLOSION_END_RADIUS * progress);
    shape.setOrigin(shape.getRadius(), shape.getRadius());

    // Alfadegreetä pienennetään edetessä
    sf::Color color(255, 165, 0); // Oranssi
    color.a = static_cast<sf::Uint8>(255 * (1.0f - progress * Config::EXPLOSION_FADE_RATIO)); // Fade out
    shape.setFillColor(color); 
}


void Explosion::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

bool Explosion::isFinished() const {
    return timePassed >= duration;
}
