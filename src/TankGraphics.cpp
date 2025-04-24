#include "Tank.hpp"
#include <cmath>  // Tämä tarvitaan trigonometristen funktioiden käyttöön
#include <iostream>  // Debug-tulostukset

/*====================
    TANKIN KONSTRUKTORI
=======================*/
Tank::Tank() : angle(45.0f), power(50.0f), hp(100), fuel(20), destroyed(false) {
    // Määritetään alkuperäinen sijainti (esimerkiksi X = 350, Y = 260)
    initialPosition = sf::Vector2f(350, 260);

    // Ladataan tekstuurit
    if (!lowerBodyTexture.loadFromFile("assets/images/lowerBody.png")) {
        std::cerr << "Error: Could not load lowerBody.png\n";
    }
    if (!upperBodyTexture.loadFromFile("assets/images/upperBody.png")) {
        std::cerr << "Error: Could not load upperBody.png\n";
    }
    if (!turretTexture.loadFromFile("assets/images/turret.png")) {
        std::cerr << "Error: Could not load turret.png\n";
    }

    // Yläosa: suorakaide ja tekstuuri
    upperBody.setSize(sf::Vector2f(60, 15));
    upperBody.setTexture(&upperBodyTexture); // Lisätään tekstuuri
 //   upperBody.setScale(2.0f, 2.0f);         // Skaalataan 2x
    upperBody.setPosition(initialPosition);

    // Alaosa: Suorakaide ja tekstuuri
    lowerBody.setSize(sf::Vector2f(60, 15));
    lowerBody.setTexture(&lowerBodyTexture);
//    lowerBody.setScale(1.2f, 1.2f); // Skaalaus

    // Sijoitus yläosan sijainnin perusteella
    lowerBody.setPosition(
        upperBody.getPosition().x + 25, 
        upperBody.getPosition().y + (-120000000000000 * upperBody.getScale().y) 
    );

    // Tykki: Suorakaide ja tekstuuri
    turret.setSize(sf::Vector2f(35, 8));
    turret.setTexture(&turretTexture); // Lisätään tekstuuri
    turret.setOrigin(0, 4); // Asetetaan kiertopiste tykin keskipisteeseen
    turret.setScale(1.2f, 1.2f);         // Skaalataan 2x
    turret.setPosition(upperBody.getPosition().x + 25, upperBody.getPosition().y);
    turret.setRotation(angle); // Asetetaan kulma



}

void Tank::draw(sf::RenderWindow &window) {
    window.draw(lowerBody);  // Piirrä telat ensin
    window.draw(upperBody);  // Piirrä tankin yläosa
    window.draw(turret);     // Piirrä tykki lopuksi
}

/*=====================
    TANKIN KULMA
=======================*/
sf::Vector2f Tank::getAimPoint(float length) const {
    // Sama kulmalasku kuin shoot()‑metodissa
    float rad = (angle - 90.f) * 3.14159265f / 180.f;
    return {
        turret.getPosition().x + std::cos(rad) * length,
        turret.getPosition().y + std::sin(rad) * length
    };
}


