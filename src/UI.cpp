#include "UI.hpp"
#include <cmath>
#include <iostream>
#include "Game.hpp"  // Tarvitaan Game-luokan määrittely
#include <Config.hpp>

// Tuulipalkin sijainti ja koko
namespace {
    const sf::Vector2f WIND_BAR_POS(80.f, 70.f);
    const sf::Vector2f WIND_BAR_SIZE(180.f, 20.f);
}

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
void UI::drawWindText(sf::RenderWindow &window, sf::Font &font, float ) {
    {
        sf::Text lbl("Tuuli: ", font, 20);
        lbl.setFillColor(sf::Color::White);
    
        // n. 60 px vasemmalle itse palkista
        lbl.setPosition(WIND_BAR_POS.x - 70.f,
                        WIND_BAR_POS.y - 2.f);
    
        window.draw(lbl);
    }
}

/* ==================================
🔥  Tuuli-indikaattori palkki
=================================== */
void UI::drawWindBarIndicator(sf::RenderWindow& window, float windForce) 
{

    // Piirretään harmaa taustapalkki ja määritetään sen koko sekä sijainti
    sf::RectangleShape bg(WIND_BAR_SIZE);
    bg.setFillColor(sf::Color(60, 60, 60)); // harmaa
    bg.setPosition(WIND_BAR_POS);
    window.draw(bg);

  
    float maxWind   = static_cast<float>(std::max(std::abs((float)Config::WIND_MIN),
                                                  std::abs((float)Config::WIND_MAX)));
    float ratio     = std::abs(windForce) / maxWind; // 0..1
    ratio           = std::fmin(ratio, 1.f);         // clamp

   
    float halfWidth = WIND_BAR_SIZE.x / 2.f; 
    float fillWidth = ratio * halfWidth; // Kuinka monta pikseliä täytetään
    

    sf::Uint8 r = static_cast<sf::Uint8>(255 * ratio);
    sf::Uint8 b = static_cast<sf::Uint8>(255 * (1.f - ratio));
    sf::Color fillColor(r, 0, b);  // punasiniskaala

    sf::RectangleShape barFill;
    barFill.setFillColor(fillColor);
    barFill.setSize(sf::Vector2f(fillWidth, WIND_BAR_SIZE.y));

    // Palkin keskipiste (x)
    float centerX = WIND_BAR_POS.x + halfWidth;

    if (windForce > 0) {
        // Täytetään oikealle päin
        barFill.setPosition(centerX, WIND_BAR_POS.y);
    }
    else if (windForce < 0) {
        // Täytetään vasemmalle, mutta fillRect haluaa positiivisen leveysarvon
        // Siispä sijoitetaan x = centerX - fillWidth
        barFill.setPosition(centerX - fillWidth, WIND_BAR_POS.y);
    }
    else {
        barFill.setSize(sf::Vector2f(2.f, WIND_BAR_SIZE.y)); 
        barFill.setPosition(centerX - 1, WIND_BAR_POS.y);
        barFill.setFillColor(sf::Color::White);
    }

    window.draw(barFill);

    // Nuolten piirtäminen
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
        float spacing = fillWidth / (arrowCount + 1);
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

        arrowShape.setPosition(arrowX, WIND_BAR_POS.y + (WIND_BAR_SIZE.y/2.f)); 
        arrowShape.setRotation(arrowDirection);

        window.draw(arrowShape);
    }
}


// Piirretään vuoroteksti
void UI::drawTurnText(sf::RenderWindow &window, sf::Font &font, const EventManager &eventManager) {
    sf::Text turnText("Vuoro: " + std::string((eventManager.getCurrentTurn() == 0) ? "Tank 1" : "Tank 2"), font, 20);
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
    hpText.setPosition(tank.getPosition().x - 20, tank.getPosition().y - 45);
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
    fuelBarBackground.setPosition(tank.getPosition().x - 20, tank.getPosition().y - 80);

    sf::RectangleShape fuelBar(sf::Vector2f(40 * fuelRatio, 5)); // Itse polttoainepalkki
    fuelBar.setFillColor(sf::Color::Green); // Vihreä palkki
    fuelBar.setPosition(tank.getPosition().x - 20, tank.getPosition().y - 80);

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
    fuelDisplay.setPosition(tank.getPosition().x - 5, tank.getPosition().y - 95);
    window.draw(fuelDisplay);
}

/* --------------------------
Piirretään tähtäin
------------------------- */
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

/* -----------------------------------------------
  Vuoroteksti tankin yläpuolelle
 --------------------------------------------- */
 void UI::drawTurnLabelOverTank(sf::RenderWindow& window,
        sf::Font&         font,
        const EventManager& ev,
        const Tank&        t1,
        const Tank&        t2)
    {
    const Tank& active = (ev.getCurrentTurn() == 0) ? t1 : t2;
    if (active.isDestroyed()) return;

    // — Värit —
    const sf::Color T1_COL{  30, 140, 255 };   // sininen
    const sf::Color T2_COL{ 255,  80,  80 };   // punainen
    sf::Color labelColor = (ev.getCurrentTurn() == 0) ? T1_COL : T2_COL;

    // Teksti
    std::string label = (ev.getCurrentTurn() == 0) ? "Tank 1" : "Tank 2";
    sf::Text txt(label, font, 20);
    txt.setFillColor(labelColor);        // Väri
    txt.setOutlineColor(sf::Color::Black); // Musta ääriviiva

    // Sijoitus (kuten aiemmin)
    sf::Vector2f pos = active.getPosition();
    float hpX = pos.x - 20.f;
    float hpY = pos.y - 50.f;
    txt.setPosition(hpX + 20.f - txt.getLocalBounds().width * 0.5f,
    hpY - 20.f);

    window.draw(txt);
    }

