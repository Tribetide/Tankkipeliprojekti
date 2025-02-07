#include <iostream>
#include "Game.hpp"
#include "Tank.hpp"

Game::Game() : window(sf::VideoMode(1920, 1080), "Rikkoutuva maasto ja tankki"), gravity(0.0005f) {
    terrain.initialize();
    tank.placeOnTerrain(terrain);
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
        }
    }
}




void Game::update() {
    for (auto &p : projectiles) {
        p.update(gravity, terrain);
    }
}

void Game::render() {
    window.clear();
    terrain.draw(window);
    tank.draw(window);

    for (auto &p : projectiles) {
        if (p.alive) window.draw(p.shape);
    }

    // Näytetään UI (kulma ja voima)
    sf::Font font;

    // Yritetään löytää fontti oletuspolusta
    if (!font.loadFromFile("assets/fonts/arial.ttf")) { 
        std::cerr << "Fontin lataus epäonnistui! Etsitty polusta: ../../assets/fonts/arial.ttf" << std::endl;
    }


    sf::Text angleText("Kulma: " + std::to_string((int)tank.getAngle()), font, 20);
    angleText.setPosition(10, 10);
    window.draw(angleText);

    sf::Text powerText("Voima: " + std::to_string((int)tank.getPower()), font, 20);
    powerText.setPosition(10, 40);
    window.draw(powerText);

    window.display();
}
