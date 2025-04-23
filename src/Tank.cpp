#include "Tank.hpp"
#include "Terrain.hpp"  // Lisätty, jotta voidaan käyttää Terrain-olioita
#include "Config.hpp"
#include "Projectile.hpp"
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
 //   upperBody.setPosition(initialPosition);

    // Alaosa: Suorakaide ja tekstuuri
    lowerBody.setSize(sf::Vector2f(60, 15));
    lowerBody.setTexture(&lowerBodyTexture);
//    lowerBody.setScale(1.2f, 1.2f); // Skaalaus

    // Sijoitus yläosan sijainnin perusteella
//    lowerBody.setPosition(
//        upperBody.getPosition().x + 25, 
//        upperBody.getPosition().y + (-120000000000000 * upperBody.getScale().y) // :D Hetki piti ettiä, että mihinä vika, vaikka kuinka muutti tätä niin mikään ei muuttunut :D
//    );

    // Tykki: Suorakaide ja tekstuuri
    turret.setSize(sf::Vector2f(35, 8));
    turret.setTexture(&turretTexture); // Lisätään tekstuuri
    turret.setOrigin(0, 4); // Asetetaan kiertopiste tykin keskipisteeseen
    turret.setScale(1.2f, 1.2f);         // Skaalataan 2x
//    turret.setPosition(upperBody.getPosition().x + 25, upperBody.getPosition().y);
    turret.setRotation(angle); // Asetetaan kulma



}

void Tank::draw(sf::RenderWindow &window) {
    window.draw(lowerBody);  // Piirrä telat ensin
    window.draw(upperBody);  // Piirrä tankin yläosa
    window.draw(turret);     // Piirrä tykki lopuksi
}

sf::Vector2f Tank::getAimPoint(float length) const {
    // Sama kulmalasku kuin shoot()‑metodissa
    float rad = (angle - 90.f) * 3.14159265f / 180.f;
    return {
        turret.getPosition().x + std::cos(rad) * length,
        turret.getPosition().y + std::sin(rad) * length
    };
}


//////  TÄÄLTÄ SÄÄDETÄÄN TANKIN OSIEN SIJAINTIA KUN LIIKUTAAN


void Tank::move(float dx, Terrain &terrain, const Tank &opponent) {
    if (fuel <= 0) {
        std::cout << "[MOVE] Fuel empty, cannot move." << std::endl;
        return;
    }
    
    sf::Vector2f oldPosition = upperBody.getPosition();
    sf::Vector2f newPosition = oldPosition;
    newPosition.x += dx;
    
    // Haetaan uusi korkeus skannaamalla alhaalta ylöspäin
    float newHeight = 0.f;
    for (int i = 0; i < 1080; i++) {
        if (terrain.checkCollision(sf::Vector2f(newPosition.x + 30, i))) {
            newHeight = i;
            break;
        }
    }
    
    // Haetaan vanhan sijainnin maapinnan korkeus
    float oldHeight = 0.f;
    for (int i = 0; i < 1080; i++) {
        if (terrain.checkCollision(sf::Vector2f(oldPosition.x + 30, i))) {
            oldHeight = i;
            break;
        }
    }
    
    // Lasketaan absoluuttinen korkeusero (slope) – debug-tulostukseksi
    float slope = std::abs(newHeight - oldHeight);
    const float MAX_SLOPE = 25.0f;  // Pikselimääräinen raja, jota voidaan myös käyttää lisätarkistukseen
    std::cout << "[DEBUG] Slope (|newHeight - oldHeight|): " << slope 
              << " (Max allowed: " << MAX_SLOPE << ")" << std::endl;
    
    // Lasketaan liikeradan kulma abs(deltaY) / deltaX:n avulla.
    float deltaX = std::abs(newPosition.x - oldPosition.x);
    float deltaY = std::abs(newHeight - oldHeight);
    float angleRadians = std::atan2(deltaY, deltaX);
    float angleDegrees = angleRadians * (180.0f / 3.14159265f);
    std::cout << "[DEBUG] Tank movement angle: " << angleDegrees << " astetta" << std::endl;
    
    // Tarkistetaan liikeratojen suunta niiden avulla, että jos liikutaan ylämäkeen,
    // eli jos uuden maapinnan korkeus on pienempi kuin vanhan (newHeight < oldHeight),
    // niin sallitun ylämäen kulma asetetaan esimerkiksi 45 asteeseen.
    float heightDiff = newHeight - oldHeight; // Huom.: negatiivinen => ylämäkeen
    if (heightDiff < 0) { // Ylämäki
        const float MAX_UPHILL_ANGLE = 80.0f; // Sallitut asteet ylämäkeen
        if (angleDegrees > MAX_UPHILL_ANGLE) {
            std::cout << "[MOVE] Uphill movement too steep (" << angleDegrees 
                      << " astetta > " << MAX_UPHILL_ANGLE << " astetta). Aborting move." << std::endl;
            return;
        }
        // Lisäksi voidaan tarkistaa, ettei ylämäen pikselierotus ole liian suuri:
        if (std::abs(heightDiff) > MAX_SLOPE) {
            std::cout << "[MOVE] Uphill slope too steep (" << std::abs(heightDiff)
                      << " > " << MAX_SLOPE << "). Aborting move." << std::endl;
            return;
        }
    }
    // Jos taas heightDiff >= 0, eli maapinta laskee (tai pysyy samana),
    // sallitaan liike, vaikka kulma olisi lähellä 90°.
    
  // Lasketaan uusi y-sijainti suoraan laskettuna: pohjakorkeus - offset
  float targetY = newHeight - 40;
  std::cout << "[DEBUG] Target Y = " << targetY << std::endl;
  
  // Pehmentää pudotusta: jos tankin nykyisestä y:stä on iso ero targetY:ssa, 
  // rajoitetaan pudotus askeleeseen "maxFallStep".
  const float maxFallStep = 10.0f; // Maksimitiputuksen määrä pikseleinä per siirto
  float currentY = oldPosition.y;
  float fallDelta = targetY - currentY;
  float adjustedY = currentY;
  
  if (fallDelta > maxFallStep) {  // Laskeutuessa (alamäkeen)
      adjustedY = currentY + maxFallStep;
  } else if (fallDelta < -maxFallStep) {  // Nousussa: rajoitetaan nousu
      adjustedY = currentY - maxFallStep;
  } else {
      adjustedY = targetY;
  }
  std::cout << "[DEBUG] Adjusted Y = " << adjustedY << std::endl;
  
  // Tarkistetaan tuleva bounding box törmäysten varalta
  sf::FloatRect futureBounds = this->getBounds();
  futureBounds.left += dx;
  futureBounds.top  += adjustedY - upperBody.getPosition().y;
  
  if (futureBounds.intersects(opponent.getBounds())) {
      std::cout << "[MOVE] Aborting move: collision with opponent." << std::endl;
      return;
  }
  
  // Päivitetään tankin sijainti
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
  std::cout << "[MOVE] Move successful. New position: (" 
            << newPosition.x << ", " << adjustedY << "), Remaining fuel: " << fuel << std::endl;
}


//////////////////////////////////////////////////////////////////////////////////


void Tank::update(Terrain &terrain, float gravity) {
    sf::Vector2f position = upperBody.getPosition();

        // Tarkista ensin, onko tankki osunut aktiiviseen räjähdykseen
    // (Voit pitää esimerkiksi lista räjähdyksistä ja käydä niitä läpi)
    bool isUnderExplosion = false;
    

    // ... (toteuta logiikka, joka asettaa isUnderExplosion = true jos tankin ala osuu räjähdyksen alueelle)

    if (isUnderExplosion) {
        // Älä muokkaa tankin y-koordinaattia, säilytä se nykyisenä
        return;
    }


    float moveAmount = gravity/100.0f;  // Testataan suuremmalla arvolla

    sf::Vector2f checkPoint(position.x + 30, position.y + 45);

    if (checkPoint.x >= 0 && checkPoint.x < 1920 && checkPoint.y >= 0 && checkPoint.y < 1080 && 
        !terrain.checkCollision(checkPoint)) {
        // Ei maata alla: pudotaan
        upperBody.move(0, moveAmount);
        lowerBody.move(0, moveAmount);
        turret.move(0, moveAmount);

        if (!isFalling) {
            isFalling = true;
            fallStartY = position.y;
        }
    } else {
        // Ollaan maan päällä
        if (isFalling) {
            isFalling = false;
            float fallDistance = upperBody.getPosition().y - fallStartY;

            // Jos pudotaan enemmän kuin 20 pikseliä, otetaan vahinkoa
            if (fallDistance > 20.f) {
                // Asetetaan alle tietyn kynnysarvon pudotukset eivät aiheuta vahinkoa
                const float safeFallDistance = 30.f; // Esim. alle 30 px pudotus: ei vahinkoa
                const float fallMultiplier = 0.1f;   // Vähennetään vahinkoa
                int damage = 0;
                if (fallDistance > safeFallDistance) {
                    damage = static_cast<int>((fallDistance - safeFallDistance) * fallMultiplier);
                }
                damage = std::max(damage, 0);
                takeDamage(damage);
                std::cout << "Fall damage: " << damage << " (from " << fallDistance << "px drop)\n";
            }
            
        }
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

    // Asetetaan tykille alkukulmasta 45 astetta
    turret.setRotation(45.0f);

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
    else if (key == sf::Keyboard::K) {
        Tank::takeDamage(Tank::getHp());
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
//    angle = 45.0f;
    hp = 100;  
    destroyed = false; 
    fuel = 20; // Nollataan polttoaine
}
