#include "Tank.hpp"
#include "Terrain.hpp"  // Lisätty, jotta voidaan käyttää Terrain-olioita
#include <cmath>  // Tämä tarvitaan trigonometristen funktioiden käyttöön

Tank::Tank() : angle(45.0f), power(50.0f), hp(100) {
    // Määritetään alkuperäinen sijainti (esimerkiksi X = 350, Y = 260)
    initialPosition = sf::Vector2f(350, 260);  // Voit säätää tämän halutun alkusijainnin mukaan

    // Yläosa: puoliympyrä
    upperBody.setRadius(20);
    upperBody.setPointCount(20);
    upperBody.setFillColor(sf::Color(128, 0, 128));
    upperBody.setPosition(initialPosition);  // Käytetään alkuperäistä sijaintia

    // Alaosa: telojen kaltainen suorakaide
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
    window.draw(lowerBody);  // Piirrä telat ensin
    window.draw(upperBody);  // Piirrä tankin yläosa
    window.draw(turret);     // Piirrä tykki lopuksi
}

void Tank::move(float dx, Terrain &terrain) {
    sf::Vector2f oldPosition = upperBody.getPosition();
    sf::Vector2f newPosition = oldPosition;
    newPosition.x += dx;  // Uusi X-koordinaatti

    // Selvitetään uuden sijainnin korkeus
    float oldHeight = 0, newHeight = 0;

    for (int i = 0; i < 1080; i++) {
        if (terrain.checkCollision(sf::Vector2f(oldPosition.x + 30, i))) {
            oldHeight = i;  // Vanha korkeus
            break;
        }
    }

    for (int i = 0; i < 1080; i++) {
        if (terrain.checkCollision(sf::Vector2f(newPosition.x + 30, i))) {
            newHeight = i;  // Uusi korkeus
            break;
        }
    }

    // Lasketaan kaltevuus
    float slope = std::abs(newHeight - oldHeight);

    // Jos mäki on liian jyrkkä, estetään liike
    const float MAX_SLOPE = 25.0f;  // Säädä tätä sopivaksi
    if (slope > MAX_SLOPE) {
        return;  // Liian jyrkkä, ei liikuta
    }

    // Jos mäki on riittävän loiva, siirretään tankki
    float adjustedY = newHeight - 40;  // Tankin korkeus suhteessa maastoon
    upperBody.setPosition(newPosition.x, adjustedY);
    lowerBody.setPosition(newPosition.x - 15, adjustedY + 30);
    turret.setPosition(newPosition.x + 25, adjustedY);
}

void Tank::update(Terrain &terrain, float gravity) {
    sf::Vector2f position = upperBody.getPosition();
    float moveAmount = gravity * 550.0f;  // Testataan suuremmalla arvolla

    // Tarkistetaan, onko tankin alla vielä maata
    if (!terrain.checkCollision(sf::Vector2f(position.x + 30, position.y + 45))) {
        upperBody.move(0, moveAmount);
        lowerBody.move(0, moveAmount);
        turret.move(0, moveAmount);
    }
}

sf::Vector2f Tank::getPosition() const {
    return upperBody.getPosition();
}

void Tank::rotateTurret(float angleDelta) {
    angle += angleDelta;
    if (angle < 0) angle += 360;   // Jos kulma menee negatiiviseksi, lisätään 360 astetta 
    if (angle > 360) angle -= 360; // Jos kulma menee yli 360, vähennetään 360 astetta

    turret.setRotation(angle - 90.0f); // SFML käyttää myötäpäiväistä kiertoa

    // Päivitetään tykin sijainti suhteessa tankin runkoon
    turret.setPosition(upperBody.getPosition().x + 25, upperBody.getPosition().y);
}

void Tank::placeOnTerrain(Terrain &terrain, int startX) {
    int y = 0;

    // Etsitään korkein piste, jossa maasto ei ole läpinäkyvä
    for (int i = 0; i < 1080; i++) {
        if (terrain.checkCollision(sf::Vector2f(startX, i))) {
            y = i - 40; // Tankin sijoitus (jotta ei jää maaston sisään)
            break;
        }
    }

    // Varmistetaan, ettei y-arvo ole negatiivinen
    y = std::max(y, 0);

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

    // Ammuksen lähtöpaikka tykin suusta
    float muzzleX = turret.getPosition().x + std::cos(radianAngle) * 30;
    float muzzleY = turret.getPosition().y + std::sin(radianAngle) * 30;
    p.shape.setPosition(muzzleX, muzzleY);

    // Vakio lähtönopeus (hidastettu, jotta painovoima ehtii vaikuttaa)
    float baseSpeed = 2.0f;

    // Skaalataan nopeus voiman mukaan (pienellä skaalauksella)
    float speed = baseSpeed + (power / 100.0f) * 1.5f;

    p.velocity = sf::Vector2f(std::cos(radianAngle) * speed, std::sin(radianAngle) * speed);

    // Voima vaikuttaa painovoimaan, jolloin suurempi voima = pienempi pudotus
    p.setGravity(0.0005f + (100.0f - power) / 10000.0f); // Pienempi voima → suurempi painovoimavaikutus

    p.alive = true;
    return p;
}

void Tank::handleInput(sf::Keyboard::Key key, Terrain &terrain, std::vector<Projectile> &projectiles, bool &waitingForTurnSwitch, sf::Clock &turnClock) {
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
    else if (key == sf::Keyboard::K) { // 🔥 K-näppäin "tappaa" tankin (asetetaan hp nollaksi)
        setHp(0);  // Asetetaan tankin HP nollaksi
    }
}

int Tank::getHp() const {
    return hp;
}

void Tank::setHp(int newHp) {
    hp = newHp;
}


void Tank::reset(Terrain &terrain, const sf::Vector2f& startPosition) {
    // Palautetaan tankki annettuun sijaintiin ja asetetaan kaikki parametrit nollaksi
    placeOnTerrain(terrain, startPosition.x); // Käytetään annettua paikkaa
    power = 50.0f;  // Nollataan voima
    angle = 45.0f;  // Nollataan kulma
    hp = 100;  // Nollataan elämä
}

