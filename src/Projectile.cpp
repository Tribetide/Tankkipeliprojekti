#include "Projectile.hpp"

Projectile::Projectile() : alive(false) {}

void Projectile::setGravity(float gravity) {
    if (gravity >= 0.0001f && gravity <= 0.01f) {  // 🔥 Varmistetaan järkevä arvo
        gravityEffect = gravity;
    }
}

void Projectile::update(float deltaTime, Terrain &terrain, float windForce) {
    if (!alive) return;

    // 🔥 Lisätään tuulen vaikutus X-akseliin
    velocity.x += windForce;

    // 🔥 Painovoima vaikuttaa Y-akseliin
    velocity.y += gravityEffect;

    // 🔥 Päivitetään ammuksen sijainti
    shape.move(velocity);

    // 🔥 Törmäystarkistus
    if (terrain.checkCollision(shape.getPosition())) {
        alive = false;
        terrain.destroy(shape.getPosition(), 50);
    }

    // 🔥 Tuhoutuminen, jos ammus menee ruudun ulkopuolelle
    if (shape.getPosition().y > 1080) {
        alive = false;
    }
}
