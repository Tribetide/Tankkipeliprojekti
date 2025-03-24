#include <iostream>
#include "Game.hpp"
#include "UI.hpp"
#include <Config.hpp>
#include <cstdlib>  // satunnaislukujen generointi
#include <ctime>    // ajan käyttö satunnaislukujen generointiin
#include <filesystem> // C++17 tiedostopoluille
#include <cmath>  // Matematiikkakirjasto

void printCurrentWorkingDirectory() {
    std::cout << "Nykyinen hakemisto: " 
              << std::filesystem::current_path() 
              << std::endl;
}

Game::Game() 
    : window(sf::VideoMode(1920, 1080), "Rikkoutuva maasto ja tankki"),
      gravity(0.0005f),
      eventManager(tank1, tank2) 
{
    printCurrentWorkingDirectory(); // Tulostetaan nykyinen hakemisto

    // Alustetaan maasto ja tankit
    terrain.initialize();
    tank1.placeOnTerrain(terrain, 100);
    tank2.placeOnTerrain(terrain, 1800);
   

    if (!moonTexture.loadFromFile("../assets/images/moon.png")) {
        std::cerr << "Kuun tekstuurin lataus epäonnistui!" << std::endl;
    }
    moonSprite.setTexture(moonTexture);
    moonSprite.setPosition(1600, 100); // Säädä sijaintia
    moonSprite.setScale(0.6f, 0.6f);   // Säädä kokoa
    


    // 🔥 Ladataan fontti kerran
    if (!font.loadFromFile(Config::FONT_PATH_1)) {
        std::cerr << "Fontin lataus epäonnistui! Yritetään polkua: " << Config::FONT_PATH_2 << std::endl;
    
        if (!font.loadFromFile(Config::FONT_PATH_2)) {
            std::cerr << "Fontin lataus epäonnistui! Yritetään polkua: " << Config::FONT_PATH_3 << std::endl;
    
            if (!font.loadFromFile(Config::FONT_PATH_3)) {
                std::cerr << "Fontin lataus epäonnistui täysin! Tarkista polut ja varmista, että tiedosto on oikeassa paikassa." << std::endl;
            }
        }
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

        // 🔥 Estetään toiminnot, jos odotetaan vuoron vaihtoa
        if (waitingForTurnSwitch) 
            return;

            Tank &activeTank = (eventManager.getCurrentTurn() == 0) ? tank1 : tank2;

            if (event.type == sf::Event::KeyPressed) {
                activeTank.handleInput(event.key.code, terrain, projectiles, waitingForTurnSwitch, turnClock);
        }
    }
}

void Game::update() {
    if (waitingForTurnSwitch) {
        if (turnClock.getElapsedTime().asSeconds() >= 2.0f && !eventManager.anyProjectilesAlive(projectiles)) {
            eventManager.switchTurn();
            waitingForTurnSwitch = false;
            eventManager.restartTurnTimer();
        }
    }

    // 🔥 Päivitä eventManager ja anna sille projektiililista
    eventManager.update(projectiles);


    // 🔥 Päivitä aktiivinen tankki
    Tank &activeTank = (eventManager.getCurrentTurn() == 0) ? tank1 : tank2;
    activeTank.update(terrain, gravity); 

    // 🔥 Päivitä kaikki ammukset
    float deltaTime = 0.9f / 60.0f; // Oletetaan 60 FPS, voit laskea oikean ajan tarvittaessa

    terrain.update(deltaTime); // 🔥 Päivitetään tähdenlennot

    for (auto &p : projectiles) {
 //       std::cout << "Ammus y: " << p.shape.getPosition().y                   // Oli vain debuaggausta varten :D 
 //       << " | Ammus x: " << p.shape.getPosition().x << std::endl;
        p.update(gravity, terrain, windForce);
        if (p.alive) {
            eventManager.handleShot(p, terrain);
        }
    }

    // 🔥 Poista kuolleet ammukset listasta
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile &p) {
            return !p.alive;
        }),
        projectiles.end()
    );
}


void Game::render() {
    window.clear();
    window.draw(moonSprite);
    terrain.draw(window);

    tank1.draw(window);
    tank2.draw(window);

    for (auto &p : projectiles) {
        if (p.alive) window.draw(p.shape);
    }

    // Hae vuorossa oleva tankki
    Tank &currentTank = (eventManager.getCurrentTurn() == 0) ? tank1 : tank2;

    // Kutsutaan UI-piirtämisen funktioita
    UI::drawTurnText(window, font, eventManager);
    UI::drawTurnTimer(window, font, eventManager);    
    UI::drawAngleText(window, font, currentTank);
    UI::drawPowerText(window, font, currentTank);
    UI::drawWindText(window, font, windForce);
    UI::drawWindIndicator(window, windForce);

    window.display();
}
