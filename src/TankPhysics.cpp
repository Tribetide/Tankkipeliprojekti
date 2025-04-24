#include "Tank.hpp"
#include "Terrain.hpp"  // Lisätty, jotta voidaan käyttää Terrain-olioita
#include <cmath>  // Tämä tarvitaan trigonometristen funktioiden käyttöön
#include <iostream>  // Debug-tulostukset


/*=================================
    TANKIN FYSIIKKA
==================================*/


// Apu­funktio – löytää ”maan” Y-koordinaatin annetusta X:stä
static float findGroundY(float x, const Terrain& terrain, int screenH   = 1080) 
{
        //Etsi ensimmäinen kiinteä pikseli alhaalta 
        int y = screenH - 1;
        while (y >= 0 && !terrain.checkCollision(sf::Vector2f(x, (float)y)))
        --y;

        if (y < 0)  // Jos ei löydy kiinteää pikseliä, palauta ruudun korkeus
        return (float)screenH;

        // Kulje ylöspäin niin kauan kuin pikseli on kiinteä 
        while (y >= 0 && terrain.checkCollision(sf::Vector2f(x, (float)y)))
        --y;

        // Kunnes löydetään ilmapikseli, palauta seuraava pikseli
        return (float)(y + 1);
    }


void Tank::move(float dx, Terrain &terrain, const Tank &opponent) {

    if (fuel <= 0) return;  // Ei voi liikkua, jos polttoaine loppu
    if (fuel <= 0) {
        std::cout << "[MOVE] Fuel empty, cannot move." << std::endl;
        return;
    }
    

    sf::Vector2f oldPosition = upperBody.getPosition();
    sf::Vector2f newPosition = oldPosition;
    newPosition.x += dx;

    // Lasketaan uusi maapinnan korkeus (groundY) uuden X:n perusteella
    float newHeight = findGroundY(newPosition.x + 30.f, terrain);
    float oldHeight = findGroundY(oldPosition.x + 30.f, terrain);

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

/*=========================
    TANKIN PÄIVITYS, 
===========================*/
void Tank::update(Terrain &terrain, float gravity) {

    sf::Vector2f position = upperBody.getPosition(); // Tankin yläosan sijainti
    float moveAmount = gravity/100.0f; 
    sf::Vector2f checkPoint(position.x + 30, position.y + 45); // Tarkistetaan kohta tankin alapuolella

    if (checkPoint.x >= 0 && checkPoint.x < 1920 && checkPoint.y >= 0 && checkPoint.y < 1200 && 
        !terrain.checkCollision(checkPoint)) {
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


/*=========================
    TANKIN SIJAINTI MAASTOSSA
===========================*/
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

}

// Palauttaa tankin yläosan sijainnin
sf::Vector2f Tank::getPosition() const {
    return upperBody.getPosition();
}

/*=====================================
    GETBOUNDS (TANKIN TÖRMÄYSLAATIKKO)
======================================*/

// Palauttaa tankin törmäyslaatikon (bounding box)
sf::FloatRect Tank::getBounds() const {
    sf::FloatRect ub = upperBody.getGlobalBounds();
    sf::FloatRect lb = lowerBody.getGlobalBounds();
    sf::FloatRect tb = turret.getGlobalBounds();

    // Lasketaan tankin törmäyslaatikko yhdistämällä yläosa, alaosa ja tykki
    float left = std::min({ub.left, lb.left, tb.left});
    float top = std::min({ub.top, lb.top, tb.top});
    float right = std::max({ub.left + ub.width, 
                             lb.left + lb.width, 
                             tb.left + tb.width});
    float bottom = std::max({ub.top + ub.height, 
                             lb.top + lb.height, 
                             tb.top + tb.height});

    return sf::FloatRect(left, top, right - left, bottom - top);
}


int Tank::getFuel() const {return fuel;}
void Tank::resetFuel() {fuel = 20;}


// Resettaa tankin tilan aloitustilanteeseen
void Tank::reset(Terrain &terrain, const sf::Vector2f& startPosition) {
    // Palautetaan tankki annettuun sijaintiin ja asetetaan kaikki parametrit nollaksi
    placeOnTerrain(terrain, startPosition.x);
    power = 50.0f;  
    hp = 100;  
    destroyed = false; 
    fuel = 20; 
}

