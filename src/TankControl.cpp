#include "Tank.hpp"
#include "Terrain.hpp"  // Lisätty, jotta voidaan käyttää Terrain-olioita
#include "Config.hpp"
#include "Projectile.hpp"
#include <cmath>  // Tämä tarvitaan trigonometristen funktioiden käyttöön
#include "SoundManager.hpp"
#include <SFML/Audio.hpp> // sf::Sound
#include <iostream>  // Debug-tulostukset


/*==========================
    TYKINPIIPUN KULMA
=============================*/

void Tank::rotateTurret(float angleDelta) {
    angle += angleDelta;
    if (angle < 0) angle += 360;   
    if (angle > 360) angle -= 360;

    //Kulma myötäpäivään
    turret.setRotation(angle - 90.0f);
    // Päivitetään tykin sijainti suhteessa tankin runkoon
    turret.setPosition(upperBody.getPosition().x + 25, upperBody.getPosition().y);
}

/*================== 
    VOIMAN SÄÄTÖ JA AMPUMINEN
========================*/
void Tank::adjustPower(float powerDelta) {

    power = std::clamp(power + powerDelta, 10.f, 100.f); // Rajoitetaan voiman säätö 10–100 väliin
}

Projectile Tank::shoot() {
    SoundManager::getInstance().playSound("fire", 100.f);

    Projectile p;
    p.shape.setRadius(5.f);
    p.shape.setFillColor(sf::Color::Red);

    // Lasketaan ammuksen lähtönopeus ja suunta
    float radianAngle = (angle - 90.0f) * (3.14159265f / 180.0f); // Muutetaan kulma radiaaneiksi

    // Asetetaan ammuksen lähtöpaikka, tykin kärki
    float muzzleX = turret.getPosition().x + std::cos(radianAngle) * 30;
    float muzzleY = turret.getPosition().y + std::sin(radianAngle) * 30;
    p.shape.setPosition(muzzleX, muzzleY);

    // Lasketaan ammuksen nopeus
    float speed = Config::BASE_PROJECTILE_SPEED
     + (power / 100.0f) * Config::MAX_EXTRA_PROJECTILE_SPEED; // Nopeus riippuu voiman säädöstä
     
    p.velocity = sf::Vector2f(std::cos(radianAngle) * speed, std::sin(radianAngle) * speed); // Ammuksen nopeus ja suunta

    // Ammuksen "elinvoimaisuus"
    p.alive = true;
    return p;

    
}

/*===========================
    TANKIN KONTROLLIT
===============================*/

void Tank::toggleControlMode() {
    mouseControlEnabled = !mouseControlEnabled;
}

void Tank::handleInput(sf::Keyboard::Key key, Terrain &terrain, 
                        std::vector<Projectile> &projectiles, bool &waitingForTurnSwitch, 
                        sf::Clock &turnClock, const Tank &opponent) {

    if (key == sf::Keyboard::Left)
        rotateTurret(-5.0f);  // Kääntää tykkiä vasemmalle
    else if (key == sf::Keyboard::Right)
        rotateTurret(5.0f);   // Kääntää tykkiä oikealle
    else if (key == sf::Keyboard::Up)
        adjustPower(5.0f);   // Lisää ammuksen lähtövoimaa
    else if (key == sf::Keyboard::Down)
        adjustPower(-5.0f);  // Vähentää ammuksen lähtövoimaa
    else if (key == sf::Keyboard::A)
        move(-5.0f, terrain, opponent);  // Siirtää tankkia vasemmalle, huomioiden maaston
    else if (key == sf::Keyboard::D)
        move(5.0f, terrain, opponent);   // Siirtää tankkia oikealle, huomioiden maaston
    else if (key == sf::Keyboard::Space) { // Ammus laukaistaan
        projectiles.push_back(shoot());  // Luodaan uusi ammus ja lisätään se projektiilien listaan
        turnClock.restart();  // Nollataan ajastin seuraavaa vuoroa varten
        waitingForTurnSwitch = true; // Odotetaan vuoron vaihtoa
    }
    else if (key == sf::Keyboard::M) {
        toggleControlMode();
    }
}

// -- Hiiriohjaus
void Tank::handleMouseInput(sf::RenderWindow &window, std::vector<Projectile> &projectiles,
                            bool &waitingForTurnSwitch, sf::Clock &turnClock) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f tankPos = upperBody.getPosition();

    // Älä säädä kulmaa jos oikea hiirinappi on pohjassa
    if (!sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        float dx = mousePos.x - (tankPos.x + 25);
        float dy = mousePos.y - tankPos.y;
        float newAngle = std::atan2(dy, dx) * 180.0f / 3.14159f;

        angle = newAngle + 90;
        if (angle < 0) angle += 360.0f; // Normalisointi 0–360
        else if (angle >= 360.0f) angle -= 360.0f;
        
        turret.setRotation(angle - 90.0f);

        mouseDragStartY = -1.f; // Nollataan drag-tila kun ei paineta oikeaa
    } 
    else {
        // Jos oikea hiirinappi on juuri painettu alas
        if (mouseDragStartY < 0)
            mouseDragStartY = static_cast<float>(mousePos.y);

            // Lasketaan pystysuuntainen liike ja säädetään voimaa
            float dy = static_cast<float>(mousePos.y) - mouseDragStartY;
            adjustPower(-dy / 10.0f);  // Negatiivinen koska y kasvaa alaspäin
            mouseDragStartY = static_cast<float>(mousePos.y); // Päivitä uuteen
    }

    // Vasemmalla hiirinapilla ammutaan
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        projectiles.push_back(shoot());
        turnClock.restart();
        waitingForTurnSwitch = true;
        mouseDragStartY = -1.f; // Resetoi vedon tilan myös laukaisussa
    }
}


/*====================
    HP JA TUHOUTUMINEN
========================*/

void Tank::takeDamage(int damage)
{
    hp -= damage;
    if (hp <= 0) {
        hp = 0;

        // Tuhotaan tankki, jos HP on 0
        if (!destroyed) {                 
            destroyed = true;
            SoundManager::getInstance()
                .playSound("tank_destroyed", 100.f);   // 🔊
        }
    }
}

void Tank::heal(int amount) {
    hp += amount;
    if (hp > 100) hp = 100; // Maksimi HP
}

int Tank::getHp() const { return hp; }
float Tank::getAngle() const { return angle; }
float Tank::getPower() const { return power; }

