#include "Tank.hpp"
#include "Terrain.hpp" // 🔥 Lisätty, jotta voidaan käyttää Terrain-olioita
#include <cmath>  // Tämä tarvitaan trigonometristen funktioiden käyttöön

Tank::Tank() : angle(45.0f), power(50.0f) {
    // 🔥 Yläosa: puoliympyrä
    upperBody.setRadius(20);
    upperBody.setPointCount(20);
    upperBody.setFillColor(sf::Color(128, 0, 128));
    upperBody.setPosition(350, 260);

    // 🔥 Alaosa: telojen kaltainen suorakaide
    lowerBody.setSize(sf::Vector2f(60, 15));
    lowerBody.setFillColor(sf::Color(128, 0, 128));
    lowerBody.setPosition(upperBody.getPosition().x - 15, upperBody.getPosition().y + 30);

    // 🔥 Tykki
    turret.setSize(sf::Vector2f(35, 8));
    turret.setFillColor(sf::Color::Blue);
    turret.setOrigin(0, 4);
    turret.setPosition(upperBody.getPosition().x + 25, upperBody.getPosition().y); // 🔥 Korjattu body → upperBody
    turret.setRotation(angle);
}

void Tank::draw(sf::RenderWindow &window) {
    window.draw(lowerBody); // 🔥 Piirrä telat ensin
    window.draw(upperBody); // 🔥 Piirrä tankin yläosa
    window.draw(turret); // 🔥 Piirrä tykki lopuksi
}


void Tank::move(float dx) {
    upperBody.move(dx, 0);
    lowerBody.move(dx, 0);
    turret.move(dx, 0);
}

sf::Vector2f Tank::getPosition() const {
    return upperBody.getPosition();
}


void Tank::rotateTurret(float angleDelta) {
    angle += angleDelta;
    if (angle < 0) angle += 360;   // Jos kulma menee negatiiviseksi, lisätään 360 astetta 
    if (angle > 360) angle -= 360; // Jos kulma menee yli 360, vähennetään 360 astetta

    turret.setRotation(angle - 90.0f); // 🔥 SFML käyttää myötäpäiväistä kiertoa

    // Päivitetään tykin sijainti suhteessa tankin runkoon
    turret.setPosition(upperBody.getPosition().x + 25, upperBody.getPosition().y);
}

void Tank::placeOnTerrain(Terrain &terrain, int startX) {
//    int startX = 350; // Alkuperäinen X-sijainti
    int y = 0;

    // Etsitään korkein piste, jossa maasto ei ole läpinäkyvä
    for (int i = 0; i < 1080; i++) {
        if (terrain.checkCollision(sf::Vector2f(startX, i))) {
            y = i - 40; // Tankin sijoitus (jotta ei jää maaston sisään)
            break;
        }
    }

    // Asetetaan tankin osat uudelle paikalle
    upperBody.setPosition(startX, y);
    lowerBody.setPosition(startX - 15, y + 30);
    turret.setPosition(startX + 25, y);
}





void Tank::adjustPower(float powerDelta) {
    power += powerDelta;
    if (power < 10) power = 10;
    if (power > 100) power = 100;
}

float Tank::getAngle() const {
    return angle;
}

float Tank::getPower() const {
    return power;
}

Projectile Tank::shoot() {
    Projectile p;
    p.shape.setRadius(5.f);
    p.shape.setFillColor(sf::Color::Red);

    float radianAngle = (angle - 90.0f) * (3.14159265f / 180.0f);

    // 🔥 Ammuksen lähtöpaikka tykin suusta
    float muzzleX = turret.getPosition().x + std::cos(radianAngle) * 30;
    float muzzleY = turret.getPosition().y + std::sin(radianAngle) * 30;
    p.shape.setPosition(muzzleX, muzzleY);


    // 🔥 Vakio lähtönopeus (hidastettu, jotta painovoima ehtii vaikuttaa)
    float baseSpeed = 2.0f;

    // 🔥 Skaalataan nopeus voiman mukaan (pienellä skaalauksella)
    float speed = baseSpeed + (power / 100.0f) * 1.5f; 

    p.velocity = sf::Vector2f(std::cos(radianAngle) * speed, std::sin(radianAngle) * speed);

    // 🔥 Voima vaikuttaa painovoimaan, jolloin suurempi voima = pienempi pudotus
    p.setGravity(0.0005f + (100.0f - power) / 10000.0f); // Pienempi voima → suurempi painovoimavaikutus

    p.alive = true;
    return p;
}