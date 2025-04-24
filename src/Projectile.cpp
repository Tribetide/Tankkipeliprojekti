#include "Config.hpp"
#include "Projectile.hpp"
#include <SoundManager.hpp>
#include <SFML/Graphics.hpp>


Projectile::Projectile() : velocity(0.f, 0.f), alive(false), gravity(Config::GRAVITY) {
    shape.setRadius(Config::PROJECTILE_RADIUS);
    shape.setFillColor(sf::Color::White);
}


void Projectile::update(float deltaTime, Terrain &terrain, float windForce) {
    if (!alive) return;

    // 🔥 Perusliike
    shape.move(velocity.x * deltaTime, velocity.y * deltaTime);

    // 🔥 Painovoima (hidastuminen y-suuntaan)
    velocity.y += gravity * deltaTime;

    // Tuuli (kiihtyvyys x-suuntaan)
    velocity.x += windForce * deltaTime;

    // 🔥 Tuhoutuminen, jos ammus menee ruudun ulkopuolelle
    sf::Vector2f pos = shape.getPosition();
    if (pos.x < Config::PROJECTILE_BOUND_LEFT || pos.x > Config::PROJECTILE_BOUND_RIGHT ||
         pos.y < Config::PROJECTILE_BOUND_TOP || pos.y > Config::PROJECTILE_BOUND_BOTTOM) {
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



