#include "menu.hpp"
#include <iostream>

namespace Menu {

    // 🔷 Alkuvalikko
    int showMenu(sf::RenderWindow &window) {
        sf::Font font;
        sf::Texture backgroundTexture;
        sf::Sprite backgroundSprite;

        // Taustakuva
        if (!backgroundTexture.loadFromFile("assets/images/MenuBackground.png")) {
            return 2;
        }
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(
            static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x,
            static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y
        );

        // Fontti
        if (!font.loadFromFile("../assets/fonts/SHOWG.TTF")) {
            std::cerr << "Fontin lataus epäonnistui!\n";
            return 2;
        }

        // Tekstit
        sf::Text title("Tankkipeli", font, 50);
        title.setPosition(200, 100);
        title.setFillColor(sf::Color::White);

        sf::Text start("1. Start (S)", font, 30);
        start.setPosition(250, 200);
        start.setFillColor(sf::Color::Green);

        sf::Text quit("2. Quit (Q)", font, 30);
        quit.setPosition(250, 300);
        quit.setFillColor(sf::Color::Red);

        sf::FloatRect startBounds = start.getGlobalBounds();
        sf::FloatRect quitBounds = quit.getGlobalBounds();

        // Valikkosilmukka
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    return 2;
                }

                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(
                        sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

                    if (startBounds.contains(mousePos)) return 1;
                    if (quitBounds.contains(mousePos)) return 2;
                }

                if (event.type == sf::Event::KeyPressed) {
                    sf::Keyboard::Key code = event.key.code;

                    if (code == sf::Keyboard::Num1 || code == sf::Keyboard::S)
                        return 1; // Start
                    if (code == sf::Keyboard::Num2 || code == sf::Keyboard::Q)
                        return 2; // Quit
                }
            }

            // Piirretään
            window.clear();
            window.draw(backgroundSprite);
            window.draw(title);
            window.draw(start);
            window.draw(quit);
            window.display();
        }

        return 2; // Ikkuna suljettiin
    }

    // 🔷 Pause-valikko pelin sisällä
    int showPauseMenu(sf::RenderWindow &window) {
        sf::Font font;
        if (!font.loadFromFile("../assets/fonts/SHOWG.TTF")) {
            return 3;
        }

        // Valikkotekstit
        sf::Text continueText("1. Continue (C)", font, 30);
        continueText.setPosition(250, 200);
        continueText.setFillColor(sf::Color::Green);

        sf::Text newGameText("2. New Game (N)", font, 30);
        newGameText.setPosition(250, 300);
        newGameText.setFillColor(sf::Color::Cyan);

        sf::Text quitText("3. Quit (Q)", font, 30);
        quitText.setPosition(250, 400);
        quitText.setFillColor(sf::Color::Red);

        sf::FloatRect contBounds = continueText.getGlobalBounds();
        sf::FloatRect newBounds = newGameText.getGlobalBounds();
        sf::FloatRect quitBounds = quitText.getGlobalBounds();

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) return 3;

                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    if (contBounds.contains(mousePos)) return 1;
                    if (newBounds.contains(mousePos)) return 2;
                    if (quitBounds.contains(mousePos)) return 3;
                }

                if (event.type == sf::Event::KeyPressed) {
                    sf::Keyboard::Key code = event.key.code;

                    if (code == sf::Keyboard::Num1 || code == sf::Keyboard::C) return 1; // Continue
                    if (code == sf::Keyboard::Num2 || code == sf::Keyboard::N) return 2; // New Game
                    if (code == sf::Keyboard::Num3 || code == sf::Keyboard::Q) return 3; // Quit
                }
            }

            window.clear(sf::Color(50, 50, 50));
            window.draw(continueText);
            window.draw(newGameText);
            window.draw(quitText);
            window.display();
        }

        return 3;
    }
}
