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
};

#endif // UI_HPP
