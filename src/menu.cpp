#include "menu.hpp"
#include <iostream>

namespace Menu {
    int showMenu(sf::RenderWindow &window) {
        sf::Font font;
        sf::Texture backgroundTexture;
        sf::Sprite backgroundSprite;

        // 🔁 Taustakuvan lataus
        if (!backgroundTexture.loadFromFile("assets/images/MenuBackground.png")) {
            return 2;
        }
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(
            static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x,
            static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y
        );
        // 🔁 Fontin lataus
        if (!font.loadFromFile("../assets/fonts/SHOWG.TTF")) {
            std::cerr << "Fontin lataus epäonnistui!\n";
            return 2;
        }

        // 🔤 Valikkotekstit
        sf::Text title("Tankkipeli", font, 50);
        title.setPosition(200, 100);
        title.setFillColor(sf::Color::White);

        sf::Text start("1. Start", font, 30);
        start.setPosition(250, 200);
        start.setFillColor(sf::Color::Green);

        sf::Text quit("2. Quit", font, 30);
        quit.setPosition(250, 300);
        quit.setFillColor(sf::Color::Red);

        sf::FloatRect startBounds = start.getGlobalBounds();
        sf::FloatRect quitBounds = quit.getGlobalBounds();

        // 🔁 Silmukka
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
                    if (event.key.code == sf::Keyboard::Num1) return 1;
                    if (event.key.code == sf::Keyboard::Num2) return 2;
                }
            }

            // 🔁 Piirretään kaikki
            window.clear();
            window.draw(backgroundSprite); // ✅ Tausta ensin
            window.draw(title);
            window.draw(start);
            window.draw(quit);
            window.display();
        }

        return 2; // Ikkuna suljettiin
    }
}
