#include "Projectile.hpp"
#include <SoundManager.hpp>


Projectile::Projectile() : velocity(0.f, 0.f), alive(false), gravity(0.0005f) {
    shape.setRadius(5.f);
    shape.setFillColor(sf::Color::White);
}

void Projectile::setGravity(float g) {
    gravity = g;
}

void Projectile::update(float deltaTime, Terrain &terrain) {
    if (!alive) return;

    // 🔥 Perusliike
    shape.move(velocity.x * deltaTime, velocity.y * deltaTime);

    // 🔥 Painovoima vaikuttaa Y-akseliin
    velocity.y += gravity * (1000.f * deltaTime); 

    // 🔥 Tuhoutuminen, jos ammus menee ruudun ulkopuolelle
    sf::Vector2f pos = shape.getPosition();
    if (pos.x < -50 || pos.x > 2000 || pos.y < -50 || pos.y > 1200) {
        alive = false;
    }
}


// Piirtää ammuksen
void Projectile::draw(sf::RenderWindow &window) const {
    if (alive) {
        window.draw(shape);
    }
}

// Palauttaa ammuksen rajat (FloatRect), käytetään intersects() -tarkistuksessa
sf::FloatRect Projectile::getBounds() const {
    return shape.getGlobalBounds();
}



