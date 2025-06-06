#ifndef UI_HPP
#define UI_HPP

#include <SFML/Graphics.hpp>
#include "Tank.hpp"
#include "EventManager.hpp"
#include "Game.hpp"

class UI {
public:
    static void drawTurnText(sf::RenderWindow &window, sf::Font &font, const EventManager &eventManager);
    static void drawTurnTimer(sf::RenderWindow &window, sf::Font &font, const EventManager &eventManager);
    static void drawAngleText(sf::RenderWindow &window, sf::Font &font, const Tank &currentTank);
    static void drawPowerText(sf::RenderWindow &window, sf::Font &font, const Tank &currentTank);
    static void drawWindText(sf::RenderWindow &window, sf::Font &font, float windForce);
    static void drawWindIndicator(sf::RenderWindow &window, float windForce);
    static void drawTankHp(sf::RenderWindow &window, sf::Font &font, const Tank &tank);
    static void drawFuelMeter(sf::RenderWindow &window, sf::Font &font, const Tank &tank);
    static void drawWindBarIndicator(sf::RenderWindow &window, float windForce);
    static void drawCrosshair(sf::RenderWindow &window, const sf::Vector2f &pos);
    static void drawTurnLabelOverTank(sf::RenderWindow& window,
        sf::Font&         font,
        const EventManager& ev,
        const Tank&        t1,
        const Tank&        t2);

};

#endif // UI_HPP
