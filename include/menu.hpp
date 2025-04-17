#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>

namespace Menu {
    // Alkuvalikko, jossa esim. Start / Quit
    int showMenu(sf::RenderWindow &window);

    // Pelin sisäinen pause-valikko: Continue / New Game / Quit
    int showPauseMenu(sf::RenderWindow &window);
}

#endif
