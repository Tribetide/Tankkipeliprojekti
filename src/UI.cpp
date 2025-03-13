#include "UI.hpp"
#include <cmath>

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