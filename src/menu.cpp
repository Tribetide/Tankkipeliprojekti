#include "menu.hpp"
#include <iostream>

namespace Menu {
    int showMenu(sf::RenderWindow &window) {
        sf::Font font;
        if (!font.loadFromFile("../assets/fonts/arial.ttf")) {
            std::cerr << "Fontin lataus epäonnistui!\n";
            return 2; // Suljetaan peli, jos fontti ei lataudu          // aika kova ehto :D 
        }

        sf::Text title("Tankkipeli", font, 50);
        title.setPosition(200, 100);
        title.setFillColor(sf::Color::White);

        sf::Text start("1. Start", font, 30);
        start.setPosition(250, 200);
        start.setFillColor(sf::Color::Green);

        sf::Text quit("2. Quit", font, 30);
        quit.setPosition(250, 300);
        quit.setFillColor(sf::Color::Red);

        // Tämä määrittää, mitkä alueet ovat painettavissa
        sf::FloatRect startBounds = start.getGlobalBounds();
        sf::FloatRect quitBounds = quit.getGlobalBounds();

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    return 2;
                }

                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        // Tarkistetaan, onko hiiri klikannut Start tai Quit
                        if (startBounds.contains(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)))) {
                            return 1; // Start valittu
                        } else if (quitBounds.contains(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)))) {
                            return 2; // Quit valittu
                        }
                    }
                }
            

                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Num1) {
                        return 1; // Start valittu näppäimellä 1
                    } else if (event.key.code == sf::Keyboard::Num2) {
                        return 2; // Quit valittu näppäimellä 2
                    }
                }
            }

            window.clear();
            window.draw(title);
            window.draw(start);
            window.draw(quit);
            window.display();
        }

        return 2; // Jos ikkuna suljetaan
    }
}

