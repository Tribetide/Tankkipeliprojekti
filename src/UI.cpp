#include "UI.hpp"
#include <cmath>
#include <iostream>
#include "Game.hpp"  // Tarvitaan Game-luokan määrittely
#include <Config.hpp>


// Piirretään kulmateksti
void UI::drawAngleText(sf::RenderWindow &window, sf::Font &font, const Tank &currentTank) {
    sf::Text angleText("Kulma: " + std::to_string(static_cast<int>(currentTank.getAngle())), font, 20);
    angleText.setPosition(10, 10);
    window.draw(angleText);
}

// Piirretään voimateksti
void UI::drawPowerText(sf::RenderWindow &window, sf::Font &font, const Tank &currentTank) {
    sf::Text powerText("Voima: " + std::to_string(static_cast<int>(currentTank.getPower())), font, 20);
    powerText.setPosition(10, 40);
    window.draw(powerText);
}

// 🔥 Pyöristetään tuulen arvo kokonaisluvuksi
void UI::drawWindText(sf::RenderWindow &window, sf::Font &font, float windForce) {
    int windValue = static_cast<int>(windForce);
    
    // 🔥 Näytetään kokonaislukuna ruudulla
    sf::Text windText("Tuuli: " + std::to_string(windValue), font, 20);
    windText.setPosition(10, 70);
    window.draw(windText);
}

// 🔥 Piirretään tuuli-indikaattori palkki
void UI::drawWindBarIndicator(sf::RenderWindow& window, float windForce) 
{
    // 1) Määritellään palkin koko ja sijainti ruudulla
    sf::Vector2f barPos(70.f, 120.f);   // Piirretään esim. (70,120)
    sf::Vector2f barSize(200.f, 20.f);  // 200 leveys, 20 korkeus

    // 2) Piirretään harmaa taustapalkki
    sf::RectangleShape barBackground(barSize);
    barBackground.setFillColor(sf::Color(60,60,60)); // harmaa
    barBackground.setPosition(barPos);
    window.draw(barBackground);

    // 3) Lasketaan, miten iso osa palkista “täytetään”.
    //    Oletus: max tuuli on ±10 (Config::WIND_MIN..WIND_MAX).
    float maxWind   = static_cast<float>(std::max(std::abs((float)Config::WIND_MIN),
                                                  std::abs((float)Config::WIND_MAX)));
    float ratio     = std::abs(windForce) / maxWind; // 0..1
    ratio           = std::fmin(ratio, 1.f);         // clamp

    // 4) Piirretään värillinen osa
    //    - Jos windForce > 0, täytetään palkin oikea puoli [keski -> oikea].
    //    - Jos windForce < 0, täytetään palkin vasen puoli [keski -> vasen].
    //    - Jos windForce == 0, ei täyttöä (tai hyvin pieni).
    float halfWidth = barSize.x / 2.f; 
    float fillWidth = ratio * halfWidth; // Kuinka monta pikseliä täytetään
    
    // Keksitään vaikka, että kova tuuli = punainen, heikko tuuli = sininen
    // (tai vihreä). Alla pieni esimerkki gradientista punainen<->sininen.
    // Voit säätää makusi mukaan.
    // ratio=1 => punainen, ratio=0 => sininen.
    sf::Uint8 r = static_cast<sf::Uint8>(255 * ratio);
    sf::Uint8 b = static_cast<sf::Uint8>(255 * (1.f - ratio));
    sf::Color fillColor(r, 0, b);  // punasiniskaala

    sf::RectangleShape barFill;
    barFill.setFillColor(fillColor);
    barFill.setSize(sf::Vector2f(fillWidth, barSize.y));

    // Palkin keskipiste (x)
    float centerX = barPos.x + halfWidth;

    if (windForce > 0) {
        // Täytetään oikealle päin
        barFill.setPosition(centerX, barPos.y);
    }
    else if (windForce < 0) {
        // Täytetään vasemmalle, mutta fillRect haluaa positiivisen leveysarvon
        // Siispä sijoitetaan x = centerX - fillWidth
        barFill.setPosition(centerX - fillWidth, barPos.y);
    }
    else {
        // windForce == 0
        // Ei täyttöä, tai pieni "nolla"
        barFill.setSize(sf::Vector2f(2.f, barSize.y)); 
        barFill.setPosition(centerX - 1, barPos.y);
        barFill.setFillColor(sf::Color::White);
    }

    window.draw(barFill);

    // 5) Piirretään nuolet. Toteutetaan toistona.
    //    Pienet kolmiot sisäkkäin, esim. 10 px välein. 
    //    Jos windForce>0 => nuolet osoittavat oikealle, <0 => vasemmalle.
    
    // Montako nuolta? Sovitaan 1..5, esim. tuuliratio * 5
    int arrowCount = static_cast<int>(std::round(ratio * 5.f));
    if (arrowCount < 1 && windForce != 0) arrowCount = 1; 
    // Asetetaan suunta
    float arrowDirection = (windForce >= 0) ? 0.f : 180.f; 

    // Pienet nuolikolmiot
    sf::ConvexShape arrowShape;
    arrowShape.setPointCount(3);
    // Piirretään nuoli oikealle, jos rotation=0 => hännän piste on vasen
    arrowShape.setPoint(0, sf::Vector2f(0.f, -3.f));
    arrowShape.setPoint(1, sf::Vector2f(6.f, 0.f));
    arrowShape.setPoint(2, sf::Vector2f(0.f, 3.f));
    arrowShape.setFillColor(sf::Color::Yellow);

    // Käydään arrowCount verran
    for (int i = 0; i < arrowCount; i++) {
        // Piirretään nuoli pieneen offsetiin
        // Sovitaan, että piirrämme nuolet tasavälein fillWidth-alueelle,
        // esim. pituus / (arrowCount+1).
        float spacing = fillWidth / (arrowCount + 1);
        // Indeksistä i+1 => etäisyys vasenta/tai oikeaa reunaa pitkin
        float dist = spacing * (i+1);

        float arrowX;
        // Kumpi suunta
        if (windForce > 0) {
            // center + dist
            arrowX = centerX + dist;
        } else if (windForce < 0) {
            // center - dist
            arrowX = centerX - dist;
        } else {
            // 0 - ei nuolia
            break;
        }

        arrowShape.setPosition(arrowX, barPos.y + (barSize.y/2.f)); 
        arrowShape.setRotation(arrowDirection);

        window.draw(arrowShape);
    }
}


// Piirretään vuoroteksti
void UI::drawTurnText(sf::RenderWindow &window, sf::Font &font, const EventManager &eventManager) {
    sf::Text turnText("Vuoro: " + std::string((eventManager.getCurrentTurn() == 0) ? "Pelaaja 1" : "Pelaaja 2"), font, 20);
    turnText.setPosition(150, 10);
    window.draw(turnText);
}

// Piirretään vuoronajan teksti
void UI::drawTurnTimer(sf::RenderWindow &window, sf::Font &font, const EventManager &eventManager) {
    sf::Text timerText("Aika: " + std::to_string(static_cast<int>(eventManager.getTimeLeft())) + "s", font, 20);
    timerText.setPosition(150, 40);
    window.draw(timerText);
}

// Piirretään tankin hp
void UI::drawTankHp(sf::RenderWindow &window, sf::Font &font, const Tank &tank) {
    sf::Text hpText(std::to_string(tank.getHp()), font, 20);
    hpText.setPosition(tank.getPosition().x - 20, tank.getPosition().y - 50);
    hpText.setFillColor(sf::Color::White); // Väriksi valkoinen
    window.draw(hpText);
}

// Piirretään tankin polttoaine
void UI::drawFuelMeter(sf::RenderWindow &window, sf::Font &font, const Tank &tank) {
    // (A) ✅ Piirretään polttoainepalkki
    float maxFuel = 20.0f; // Maksimi polttoaine
    float fuelRatio = tank.getFuel() / maxFuel; // Kuinka täysi tankki on

    sf::RectangleShape fuelBarBackground(sf::Vector2f(40, 5)); // Tausta
    fuelBarBackground.setFillColor(sf::Color(50, 50, 50)); // Harmaa tausta
    fuelBarBackground.setPosition(tank.getPosition().x - 20, tank.getPosition().y - 70);

    sf::RectangleShape fuelBar(sf::Vector2f(40 * fuelRatio, 5)); // Itse polttoainepalkki
    fuelBar.setFillColor(sf::Color::Green); // Vihreä palkki
    fuelBar.setPosition(tank.getPosition().x - 20, tank.getPosition().y - 70);

    window.draw(fuelBarBackground);
    window.draw(fuelBar);

    // (B) ✅ Piirretään tekstimuotoinen mittari ("E - 1/2 - F")
    std::string fuelText;
    if (tank.getFuel() == 0) fuelText = "E";      // Tyhjä
    else if (tank.getFuel() <= maxFuel / 4) fuelText = "1/4";
    else if (tank.getFuel() <= maxFuel / 2) fuelText = "1/2";
    else if (tank.getFuel() <= 3 * maxFuel / 4) fuelText = "3/4";
    else fuelText = "F"; // Täysi tankki

    sf::Text fuelDisplay(fuelText, font, 14);
    fuelDisplay.setFillColor(sf::Color::White);
    fuelDisplay.setPosition(tank.getPosition().x - 5, tank.getPosition().y - 90);
    window.draw(fuelDisplay);
}

void UI::drawCrosshair(sf::RenderWindow &window,
        const sf::Vector2f &pos)
    {
    
    float size = Config::CROSSHAIR_SIZE;

    // Vaaka‑ ja pystypalkit
    sf::RectangleShape h(sf::Vector2f(size, 2));
    sf::RectangleShape v(sf::Vector2f(2, size));
    h.setOrigin(size / 2, 1);
    v.setOrigin(1, size / 2);

    h.setPosition(pos);
    v.setPosition(pos);

    h.setFillColor(sf::Color::Red);
    v.setFillColor(sf::Color::Red);

    window.draw(h);
    window.draw(v);
}
