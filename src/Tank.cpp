#include "Tank.hpp"
#include "Terrain.hpp"  // Lisätty, jotta voidaan käyttää Terrain-olioita
#include <cmath>  // Tämä tarvitaan trigonometristen funktioiden käyttöön
#include "SoundManager.hpp"
#include <SFML/Audio.hpp> // sf::Sound
#include <iostream>  // Debug-tulostukset


Tank::Tank() : angle(45.0f), power(50.0f), hp(100), destroyed(false) {
    initialPosition = sf::Vector2f(350, 260);  

    // Yläosa: puoliympyrä
    upperBody.setRadius(20);
    upperBody.setPointCount(20);
    upperBody.setFillColor(sf::Color(128, 0, 128));
    upperBody.setPosition(initialPosition); 

    // Alaosa: telojen suorakaide
    lowerBody.setSize(sf::Vector2f(60, 15));
    lowerBody.setFillColor(sf::Color(128, 0, 128));
    lowerBody.setPosition(upperBody.getPosition().x - 15, upperBody.getPosition().y + 30);

    // Tykki
    turret.setSize(sf::Vector2f(35, 8));
    turret.setFillColor(sf::Color::Blue);
    turret.setOrigin(0, 4);
    turret.setPosition(upperBody.getPosition().x + 25, upperBody.getPosition().y);
    turret.setRotation(angle);
}

void Tank::draw(sf::RenderWindow &window) {
    window.draw(lowerBody);  
    window.draw(upperBody);  
    window.draw(turret);     
}


void Tank::update(Terrain &terrain, float gravity) {
    // Pudotetaan tankkia, jos se ei ole maassa
    sf::Vector2f position = upperBody.getPosition();
    

    // Tarkistetaan, onko tankin alla vielä maata
    if (!terrain.checkCollision(sf::Vector2f(position.x + 30, position.y + 45))) {
        float moveAmount = gravity * 550.0f; 
        upperBody.move(0, moveAmount);
        lowerBody.move(0, moveAmount);
        turret.move(0, moveAmount);
    }
}

void Tank::move(float dx, Terrain &terrain) {
    sf::Vector2f oldPosition = upperBody.getPosition();
    sf::Vector2f newPosition = oldPosition;
    newPosition.x += dx; 

    // Haetaan vanha korkeus
    float oldHeight = 0.f;
    for (int i = 0; i < 1080; i++) {
        if (terrain.checkCollision(sf::Vector2f(oldPosition.x + 30, i))) {
            oldHeight = i; 
            break;
        }
    }

    // Haetaan uusi korkeus
    float newHeight = 0.f;
    for (int i = 0; i < 1080; i++) {
        if (terrain.checkCollision(sf::Vector2f(newPosition.x + 30, i))) {
            newHeight = i; 
            break;
        }
    }

    // Lasketaan kaltevuus, jos mäki on liian jyrkkä, estetään liike
    float slope = std::abs(newHeight - oldHeight);
    const float MAX_SLOPE = 25.0f;
    if (slope > MAX_SLOPE) {
        return;
    }

    // Jos mäki on riittävän loiva, siirretään tankki
    float adjustedY = newHeight - 40;
    upperBody.setPosition(newPosition.x, adjustedY);
    lowerBody.setPosition(newPosition.x - 15, adjustedY + 30);
    turret.setPosition(newPosition.x + 25, adjustedY);
}

void Tank::rotateTurret(float angleDelta) {
    angle += angleDelta;
    if (angle < 0) angle += 360;   
    if (angle > 360) angle -= 360;

    //Kulma myötäpäivään
    turret.setRotation(angle - 90.0f);
    // Päivitetään tykin sijainti suhteessa tankin runkoon
    turret.setPosition(upperBody.getPosition().x + 25, upperBody.getPosition().y);
}

void Tank::adjustPower(float powerDelta) {
    power += powerDelta;
    if (power < 10) power = 10;
    if (power > 100) power = 100;
}

Projectile Tank::shoot() {
    SoundManager::getInstance().playSound("fire", 100.f);

    Projectile p;
    p.shape.setRadius(5.f);
    p.shape.setFillColor(sf::Color::Red);

    float radianAngle = (angle - 90.0f) * (3.14159265f / 180.0f);

    // Lasketaan ammuksen lähtöpaikka, tykin kärki
    float muzzleX = turret.getPosition().x + std::cos(radianAngle) * 30;
    float muzzleY = turret.getPosition().y + std::sin(radianAngle) * 30;
    p.shape.setPosition(muzzleX, muzzleY);

    // Vakio lähtönopeus (hidastettu, jotta painovoima ehtii vaikuttaa)
    float baseSpeed = 150.0f; //pikseliä sekunnissa

    // Skaalataan nopeus voiman mukaan (pienellä skaalauksella)
    float speed = baseSpeed + (power / 100.0f) * 150.f; // 150 pikseliä sekunnissa maksimissaan

    p.velocity = sf::Vector2f(std::cos(radianAngle) * speed, std::sin(radianAngle) * speed);

    // Voima vaikuttaa painovoimaan, jolloin suurempi power = pienempi painovoima
    p.setGravity(0.0005f + (100.0f - power) / 10000.0f); 

    p.alive = true;
    return p;

    
}

void Tank::placeOnTerrain(Terrain &terrain, int startX) {
    // Etsitään maan korkein kohta annetusta x-koordinaatista
    int yFound = 0;
    for (int i = 0; i < 1080; i++) {
        if (terrain.checkCollision(sf::Vector2f(startX, i))) {
            yFound = i - 40; // Tankin sijoitus (jotta ei jää maaston sisään)
            break;
        }
    }

    // Varmistetaan, ettei y-arvo ole negatiivinen
    yFound = std::max(yFound, 0);

    // Asetetaan tankin osat uudelle paikalle
    upperBody.setPosition(startX, yFound);
    lowerBody.setPosition(startX - 15, yFound + 30);
    turret.setPosition(startX + 25, yFound);
}

void Tank::handleInput(sf::Keyboard::Key key, Terrain &terrain, 
                        std::vector<Projectile> &projectiles, bool &waitingForTurnSwitch, 
                        sf::Clock &turnClock) {
    if (key == sf::Keyboard::Left)
        rotateTurret(-5.0f);  // Kääntää tykkiä vasemmalle
    else if (key == sf::Keyboard::Right)
        rotateTurret(5.0f);   // Kääntää tykkiä oikealle
    else if (key == sf::Keyboard::Up)
        adjustPower(5.0f);   // Lisää ammuksen lähtövoimaa
    else if (key == sf::Keyboard::Down)
        adjustPower(-5.0f);  // Vähentää ammuksen lähtövoimaa
    else if (key == sf::Keyboard::A)
        move(-5.0f, terrain);  // Siirtää tankkia vasemmalle, huomioiden maaston
    else if (key == sf::Keyboard::D)
        move(5.0f, terrain);   // Siirtää tankkia oikealle, huomioiden maaston
    else if (key == sf::Keyboard::Space) { // Ammus laukaistaan
        projectiles.push_back(shoot());  // Luodaan uusi ammus ja lisätään se projektiilien listaan
        turnClock.restart();  // Nollataan ajastin seuraavaa vuoroa varten
        waitingForTurnSwitch = true; // Odotetaan vuoron vaihtoa
    }

}

// -- Tuhoutuminen ja elämäpisteet

void Tank::takeDamage(int damage){
    hp -= damage;
    if (hp < 0) hp = 0;

    // Tuhotaan tankki, jos HP on 0
    if (hp == 0) {
        destroyed = true;
    }
}

void Tank::heal(int amount) {
    hp += amount;
    if (hp > 100) hp = 100; // Maksimi HP
}

int Tank::getHp() const {
    return hp;
}


//-- Getterit

float Tank::getAngle() const {
    return angle;
}

float Tank::getPower() const {
    return power;
}

sf::Vector2f Tank::getPosition() const {
    // Valitaan yläosan sijainti tankin sijainniksi
    return upperBody.getPosition();
}

// Palauttaa yhdistetyn bounding boxin yläosasta, alaosasta ja tykistä
sf::FloatRect Tank::getBounds() const {
    sf::FloatRect ub = upperBody.getGlobalBounds();
    sf::FloatRect lb = lowerBody.getGlobalBounds();
    sf::FloatRect tb = turret.getGlobalBounds();

    // Lasketaan “union” manuaalisesti, eli yhdistetään kaikki bounding boxit
    float left   = std::min({ub.left, lb.left, tb.left});
    float top    = std::min({ub.top, lb.top, tb.top});
    float right  = std::max({ub.left + ub.width, 
                             lb.left + lb.width, 
                             tb.left + tb.width});
    float bottom = std::max({ub.top + ub.height, 
                             lb.top + lb.height, 
                             tb.top + tb.height});

    return sf::FloatRect(left, top, right - left, bottom - top);
}


//-- Resetointi
void Tank::reset(Terrain &terrain, const sf::Vector2f& startPosition) {
    // Palautetaan tankki annettuun sijaintiin ja asetetaan kaikki parametrit nollaksi
    placeOnTerrain(terrain, startPosition.x); // Käytetään annettua paikkaa
    power = 50.0f;  
    angle = 45.0f;
    hp = 100;  
    destroyed = false; 
}


