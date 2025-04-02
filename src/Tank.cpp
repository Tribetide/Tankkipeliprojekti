#include "Tank.hpp"
#include "Terrain.hpp"  // Lisätty, jotta voidaan käyttää Terrain-olioita
#include <cmath>  // Tämä tarvitaan trigonometristen funktioiden käyttöön
#include <iostream>

Tank::Tank() : angle(45.0f), power(50.0f), hp(100), fuel(20) {
    // Määritetään alkuperäinen sijainti (esimerkiksi X = 350, Y = 260)
    initialPosition = sf::Vector2f(350, 260);

    // Ladataan tekstuurit
    if (!lowerBodyTexture.loadFromFile("../assets/images/lowerBody.png")) {
        std::cerr << "Error: Could not load lowerBody.png\n";
    }
    if (!upperBodyTexture.loadFromFile("../assets/images/upperBody.png")) {
        std::cerr << "Error: Could not load upperBody.png\n";
    }
    if (!turretTexture.loadFromFile("../assets/images/turret.png")) {
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
        upperBody.getPosition().y + (-120000000000000 * upperBody.getScale().y) // :D Hetki piti ettiä, että mihinä vika, vaikka kuinka muutti tätä niin mikään ei muuttunut :D
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
    window.draw(lowerBody);  // Piirrä alaosa ensin
    window.draw(upperBody);  // Piirrä tankin yläosa seuraavaksi
    window.draw(turret);     // Piirrä tykki lopuksi
}




//////  TÄÄLTÄ SÄÄDETÄÄN TANKIN OSIEN SIJAINTIA KUN LIIKUTAAN




void Tank::move(float dx, Terrain &terrain) {
    if (fuel <= 0) return;  // Ei voi liikkua, jos polttoaine loppu

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
    lowerBody.setPosition(
        upperBody.getPosition().x, 
        upperBody.getPosition().y + upperBody.getSize().y * upperBody.getScale().y
    );
    turret.setPosition(
        upperBody.getPosition().x + 25 * turret.getScale().x, 
        upperBody.getPosition().y
    );

    fuel--;  // Vähennetään polttoainetta jokaisella liikkeellä
}

//////////////////////////////////////////////////////////////////////////////////


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




////////////  TÄÄLTÄ SÄÄDETÄÄN TANKIN OSIEN SIJAINTIA KUN ASETETAAN TANKKI

void Tank::placeOnTerrain(Terrain &terrain, int startX) {
    int terrainHeight = 0;

    // Selvitetään korkein piste maastossa, jossa tankki voidaan sijoittaa
    for (int i = 0; i < 1080; i++) {
        if (terrain.checkCollision(sf::Vector2f(startX, i))) {
            terrainHeight = i; // Asetetaan alaosan korkeus suoraan maastoon
            break;
        }
    }

    // Varmistetaan, ettei korkeus ole negatiivinen
    terrainHeight = std::max(terrainHeight, 0);

    // Asetetaan alaosa maaston korkeudelle
    lowerBody.setPosition(startX, terrainHeight);

    // Yläosan sijainti suhteessa alaosaan
    upperBody.setPosition(
        lowerBody.getPosition().x, 
        lowerBody.getPosition().y - upperBody.getSize().y * upperBody.getScale().y
    );

    // Tykin sijainti suhteessa yläosaan
    turret.setPosition(
        upperBody.getPosition().x + 25 * turret.getScale().x, 
        upperBody.getPosition().y
    );

    // Debug: Tulosta tankin osien sijainnit
    std::cout << "Tank placed at X: " << startX << ", Terrain Height: " << terrainHeight << "\n";
    std::cout << "LowerBody Position: " << lowerBody.getPosition().x << ", " << lowerBody.getPosition().y << "\n";
    std::cout << "UpperBody Position: " << upperBody.getPosition().x << ", " << upperBody.getPosition().y << "\n";
    std::cout << "Turret Position: " << turret.getPosition().x << ", " << turret.getPosition().y << "\n";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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

int Tank::getFuel() const {
    return fuel;
}

void Tank::resetFuel() {
    fuel = 20;  // Uusi vuoro, polttoaine täyteen
}

void Tank::reset(Terrain &terrain, const sf::Vector2f& startPosition) {
    // Palautetaan tankki annettuun sijaintiin ja asetetaan kaikki parametrit nollaksi
    placeOnTerrain(terrain, startPosition.x); // Käytetään annettua paikkaa
    power = 50.0f;  // Nollataan voima
    angle = 45.0f;  // Nollataan kulma
    hp = 100;  // Nollataan elämä
    fuel = 20; // Nollataan polttoaine
}
