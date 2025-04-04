// Explosion.cpp
#include "Explosion.hpp"
#include <iostream>

Explosion::Explosion(sf::Vector2f position)
    : duration(10.0f), timePassed(0.0f)
{
    shape.setRadius(10.f); // Alkuarvo isommaksi
    shape.setOrigin(10.f, 10.f);
    shape.setFillColor(sf::Color::Red); // Kirkkaampi
    shape.setPosition(position);
    std::cout << "Räjähdys luotu kohtaan: " << position.x << ", " << position.y << std::endl;
}

void Explosion::update(float deltaTime) {
    timePassed += deltaTime;

    float progress = timePassed / duration;
    shape.setRadius(120.f * progress);
    shape.setOrigin(shape.getRadius(), shape.getRadius());

    // Alfadegreetä pienennetään edetessä
    sf::Color color(255, 165, 0); // Oranssi
    color.a = static_cast<sf::Uint8>(255 * (1.0f - progress * 0.7f)); // Fade out
    shape.setFillColor(color);  // ✅ Nyt alfadegree toimii oikein
}


void Explosion::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

bool Explosion::isFinished() const {
    return timePassed >= duration;
}
