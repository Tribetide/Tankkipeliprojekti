#include "Tank.hpp"
#include "Terrain.hpp"  // Lisätty, jotta voidaan käyttää Terrain-olioita
#include <cmath>  // Tämä tarvitaan trigonometristen funktioiden käyttöön
#include "SoundManager.hpp"
#include <SFML/Audio.hpp> // sf::Sound
#include <iostream>  // Debug-tulostukset


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
    window.draw(lowerBody);  // Piirrä telat ensin
    window.draw(upperBody);  // Piirrä tankin yläosa
    window.draw(turret);     // Piirrä tykki lopuksi
}




//////  TÄÄLTÄ SÄÄDETÄÄN TANKIN OSIEN SIJAINTIA KUN LIIKUTAAN




void Tank::move(float dx, Terrain &terrain, const Tank &opponent) {
    if (fuel <= 0) return;  // Ei voi liikkua, jos polttoaine loppu

    sf::Vector2f oldPosition = upperBody.getPosition();
    sf::Vector2f newPosition = oldPosition;
    newPosition.x += dx;

    // Haetaan uusi korkeus
    float newHeight = 0.f;
    for (int i = 0; i < 1080; i++) {
        if (terrain.checkCollision(sf::Vector2f(newPosition.x + 30, i))) {
            newHeight = i;
            break;
        }
    }

    // Lasketaan kaltevuus
    float oldHeight = 0.f;
    for (int i = 0; i < 1080; i++) {
        if (terrain.checkCollision(sf::Vector2f(oldPosition.x + 30, i))) {
            oldHeight = i;
            break;
        }
    }

    float slope = std::abs(newHeight - oldHeight);
    const float MAX_SLOPE = 25.0f;
    if (slope > MAX_SLOPE) return;

    // 🔥 Lasketaan tuleva bounding box ja tarkistetaan törmäys vastustajaan
    float adjustedY = newHeight - 40;

    // Temporäärinen paikka missä tankki olisi liikkeen jälkeen
    sf::FloatRect futureBounds = this->getBounds();
    float dxAll = dx;
    float dyAll = adjustedY - upperBody.getPosition().y;
    futureBounds.left += dxAll;
    futureBounds.top  += dyAll;

    // 🔥 Jos törmäys vastustajan kanssa, estä liike
    if (futureBounds.intersects(opponent.getBounds())) {
        return;
    }

    // 🔥 Jos ei törmäystä, liikutetaan tankkia
    upperBody.setPosition(newPosition.x, adjustedY);
    lowerBody.setPosition(
        upperBody.getPosition().x,
        upperBody.getPosition().y + upperBody.getSize().y * upperBody.getScale().y
    );
    turret.setPosition(
        upperBody.getPosition().x + 25 * turret.getScale().x,
        upperBody.getPosition().y
    );

    fuel--;
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
    if (angle < 0) angle += 360;   
    if (angle > 360) angle -= 360;

    //Kulma myötäpäivään
    turret.setRotation(angle - 90.0f);
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

/*void Tank::placeOnTerrain(Terrain &terrain, int startX) {
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
*/


//--- Liikkuminen näppäimillä ja/tai hiirellä ----- //

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

int Tank::getFuel() const {
    return fuel;
}

void Tank::resetFuel() {
    fuel = 20;  // Uusi vuoro, polttoaine täyteen
}

void Tank::reset(Terrain &terrain, const sf::Vector2f& startPosition) {
    // Palautetaan tankki annettuun sijaintiin ja asetetaan kaikki parametrit nollaksi
    placeOnTerrain(terrain, startPosition.x); // Käytetään annettua paikkaa
    power = 50.0f;  
    angle = 45.0f;
    hp = 100;  
    destroyed = false; 
    fuel = 20; // Nollataan polttoaine
}
