#include <iostream>
#include "Game.hpp"
#include <Config.hpp>
#include <cstdlib>  // satunnaislukujen generointi
#include <ctime>    // ajan käyttö satunnaislukujen generointiin

Game::Game() : window(sf::VideoMode(1920, 1080), "Rikkoutuva maasto ja tankki"), gravity(0.0005f) {
    terrain.initialize();
    tank.placeOnTerrain(terrain);


    // 🔥 Ladataan fontti kerran
    if (!font.loadFromFile(Config::FONT_PATH)) {
        std::cerr << "Fontin lataus epäonnistui! Etsitty polusta: " << Config::FONT_PATH << std::endl;
    }

    // 🔥 Alustetaan satunnainen tuuli (-0.002f ja 0.002f välillä)
    std::srand(std::time(nullptr));
    windForce = (std::rand() % 200 - 100) / 50000.0f;
}



void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Left)
                tank.rotateTurret(-5.0f);  // Kääntää tykkiä vasemmalle
            if (event.key.code == sf::Keyboard::Right)
                tank.rotateTurret(5.0f);   // Kääntää tykkiä oikealle
            if (event.key.code == sf::Keyboard::Up)
                tank.adjustPower(5.0f);   // Lisää voimaa
            if (event.key.code == sf::Keyboard::Down)
                tank.adjustPower(-5.0f);  // Vähentää voimaa
            if (event.key.code == sf::Keyboard::Space)
                projectiles.push_back(tank.shoot());  // Ampuu
            if (event.key.code == sf::Keyboard::A)
                tank.move(-5.0f);  // Liiku vasemmalle
            if (event.key.code == sf::Keyboard::D)
                tank.move(5.0f);   // Liiku oikealle
        }
    }
}




void Game::update() {
    for (auto &p : projectiles) {
        p.update(gravity, terrain, windForce);  // 🔥 Päivitetty versio
    }
}



void Game::render() {
    window.clear();
    terrain.draw(window);
    tank.draw(window);

    for (auto &p : projectiles) {
        if (p.alive) window.draw(p.shape);
    }


    sf::Text angleText("Kulma: " + std::to_string((int)tank.getAngle()), font, 20);
    angleText.setPosition(10, 10);
    window.draw(angleText);

    sf::Text powerText("Voima: " + std::to_string((int)tank.getPower()), font, 20);
    powerText.setPosition(10, 40);
    window.draw(powerText);

    // 🔥 Pyöristetään tuulen arvo kokonaisluvuksi
    int windValue = static_cast<int>(std::round(std::abs(windForce) * 10000));

    // 🔥 Näytetään kokonaislukuna ruudulla
    sf::Text windText("Tuuli: " + std::to_string(windValue) + " m/s", font, 20);
    windText.setPosition(10, 70);
    window.draw(windText);

    // 🔥 Piirretään tuuli-indikaattori nuolena
    drawWindIndicator();

    window.display();
}

void Game::drawWindIndicator() {
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

