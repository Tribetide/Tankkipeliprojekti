#include "Tank.hpp"
#include "Terrain.hpp"  // Lisätty, jotta voidaan käyttää Terrain-olioita
#include <cmath>  // Tämä tarvitaan trigonometristen funktioiden käyttöön
#include <iostream>  // Debug-tulostukset


/*=================================
    TANKIN FYSIIKKA
==================================*/

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

    // Hetkellinen paikka missä tankki olisi liikkeen jälkeen
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

/*=========================
    TANKIN PÄIVITYS, 
===========================*/
void Tank::update(Terrain &terrain, float gravity) {

    sf::Vector2f position = upperBody.getPosition(); // Tankin yläosan sijainti
    float moveAmount = gravity/100.0f;  // Testataan suuremmalla arvolla
    sf::Vector2f checkPoint(position.x + 30, position.y + 45); // Tarkistetaan kohta tankin alapuolella

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
                int damage = static_cast<int>((fallDistance - 50.f) * 0.2f); // esim. 1 dmg per 2 pikseliä
                damage = std::max(damage, 0); // estetään negatiivinen damage
                takeDamage(damage);
                std::cout << "💥 Fall damage: " << damage << " (from " << fallDistance << "px drop)\n";
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

}

sf::Vector2f Tank::getPosition() const {
    return upperBody.getPosition();
}

/*=====================================
    GETBOUNDS (TANKIN TÖRMÄYSLAATIKKO)
======================================*/

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

int Tank::getFuel() const {return fuel;}
void Tank::resetFuel() {fuel = 20;}

void Tank::reset(Terrain &terrain, const sf::Vector2f& startPosition) {
    // Palautetaan tankki annettuun sijaintiin ja asetetaan kaikki parametrit nollaksi
    placeOnTerrain(terrain, startPosition.x); // Käytetään annettua paikkaa
    power = 50.0f;  
//    angle = 45.0f;
    hp = 100;  
    destroyed = false; 
    fuel = 20; // Nollataan polttoaine
}

