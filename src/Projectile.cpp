#include "Projectile.hpp"

Projectile::Projectile() : alive(false) {}

void Projectile::update(float deltaTime, Terrain &terrain) {
    if (!alive) return;

    velocity.y += gravityEffect;
    shape.move(velocity);

    // 🔥 Varmista, että törmäys tarkistetaan oikein
    if (terrain.checkCollision(shape.getPosition())) {
        alive = false;
        terrain.destroy(shape.getPosition(), 50); // 🔥 Pienennä tarvittaessa tuhoutumisalueen kokoa
    }

    if (shape.getPosition().y > 1080) { // 🔥 Päivitetty ruudun alaraja
    alive = false;
    }
}

