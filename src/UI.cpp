#include "UI.hpp"
#include <cmath>

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
    int windValue = static_cast<int>(std::round(std::abs(windForce) * 10000));
    
    // 🔥 Näytetään kokonaislukuna ruudulla
    sf::Text windText("Tuuli: " + std::to_string(windValue) + " m/s", font, 20);
    windText.setPosition(10, 70);
    window.draw(windText);
}

// 🔥 Piirretään tuuli-indikaattori nuolena
void UI::drawWindIndicator(sf::RenderWindow &window, float windForce) {
    float windStrength = std::abs(windForce) * 10000;  // 🔥 Skaalataan tuulen pituus
    float startX = 50;  // 🔥 Nuolen aloituspiste
    float startY = 120;
    float endX = startX + (windForce * 10000);  // 🔥 Pituus ja suunta
    float endY = startY;

    sf::VertexArray windArrow(sf::Lines, 2);
    windArrow[0].position = sf::Vector2f(startX, startY);
    windArrow[0].color = sf::Color::White;
    windArrow[1].position = sf::Vector2f(endX, endY);
    windArrow[1].color = sf::Color::White;

    window.draw(windArrow);

    // 🔥 Lisätään iso nuolenpää (kolmio)
    sf::ConvexShape arrowHead;
    arrowHead.setPointCount(3); // 🔥 Kolmio
    arrowHead.setPoint(0, sf::Vector2f(0, -6));  // Yläosa
    arrowHead.setPoint(1, sf::Vector2f(12, 0));   // Oikea alakulma
    arrowHead.setPoint(2, sf::Vector2f(0, 6));   // Vasen alakulma
    arrowHead.setFillColor(sf::Color::White);

    // 🔥 Sijoitetaan nuolenpää oikeaan kohtaan ja käännetään tuulen suuntaan
    arrowHead.setPosition(endX, endY);
    arrowHead.setRotation((windForce >= 0) ? 0 : 180);  // 🔥 Oikea suunta

    window.draw(arrowHead);
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
