#include <iostream>
#include "Game.hpp"
#include "UI.hpp"
#include <Config.hpp>
#include <cstdlib>  // satunnaislukujen generointi
#include <ctime>    // ajan käyttö satunnaislukujen generointiin
#include <filesystem> // C++17 tiedostopoluille
#include <cmath>  // Matematiikkakirjasto
#include "menu.hpp"
#include <memory>


void printCurrentWorkingDirectory() {
    std::cout << "Nykyinen hakemisto: " 
              << std::filesystem::current_path() 
              << std::endl;
}

Game::Game() 
    : window(sf::VideoMode(1920, 1080), "Rikkoutuva maasto ja tankki"),
      gravity(0.0005f),
      eventManager(tank1, tank2, *this),
      tank1StartPosition(100, 0), // Alkuperäinen sijainti tankille 1
      tank2StartPosition(1800, 0) // Alkuperäinen sijainti tankille 2
{
    printCurrentWorkingDirectory(); // Tulostetaan nykyinen hakemisto

    // Alustetaan maasto ja tankit
    terrain.initialize();
    tank1.placeOnTerrain(terrain, tank1StartPosition.x); // Käytetään tank1:n aloitussijaintia
    tank2.placeOnTerrain(terrain, tank2StartPosition.x); // Käytetään tank2
   

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

    // 🔥 Alustetaan satunnainen tuuli (-0.0005f....0.0005f välillä)
    std::srand(std::time(nullptr));
    windForce = (std::rand() % 200 - 100) / 100000.0f;
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
            eventManager.switchTurn(windForce, *this);  // 🔥 Lisätty `*this`
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

    // Piirretään vuorossa olevan tankin hp ja polttoaine
    UI::drawTankHp(window, font, currentTank);  // Piirrä hp vain kerran
    UI::drawFuelMeter(window, font, currentTank);

    window.display();
}

void Game::endGame() {
    // Näytetään voittoteksti
    sf::Text winnerText;
    winnerText.setFont(font);
    winnerText.setCharacterSize(50);
    winnerText.setFillColor(sf::Color::Green);

    // Määritetään voittoteksti
    if (tank1.getHp() == 0) {
        winnerText.setString("Tank 2 wins!");
    } else if (tank2.getHp() == 0) {
        winnerText.setString("Tank 1 wins!");
    } else {
        winnerText.setString("It's a draw!");
    }

    winnerText.setPosition(200, 200);  // Sijoitetaan voittoteksti

    window.clear();
    window.draw(winnerText);
    window.display();

    // Pidetään voittoteksti näkyvillä 3 sekuntia
    sf::sleep(sf::seconds(3));

    // Siirrytään vaihtoehtoihin
    sf::Text retryText("1. Retry", font, 30);
    retryText.setPosition(250, 200);
    retryText.setFillColor(sf::Color::Green);

    sf::Text quitText("2. Quit", font, 30);
    quitText.setPosition(250, 300);
    quitText.setFillColor(sf::Color::Yellow);


    // Vaihtoehtojen alueiden määrittäminen
    sf::FloatRect retryBounds = retryText.getGlobalBounds();
    sf::FloatRect quitBounds = quitText.getGlobalBounds();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    // Tarkistetaan, mikä vaihtoehto valittiin
                    if (retryBounds.contains(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)))) {
                        resetGame();  // Käynnistetään peli uudelleen
                        run();
                        return; // Poistutaan endGame-metodista
                    } else if (quitBounds.contains(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)))) {
                        window.close();  // Suljetaan peli
                        return; // Poistutaan endGame-metodista
                    }
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                // Näppäimistön valinnat
                if (event.key.code == sf::Keyboard::Num1) {
                    resetGame();  // Käynnistetään peli uudelleen
                    run();
                    return;
                } else if (event.key.code == sf::Keyboard::Num2) {
                    window.close();  // Suljetaan peli
                    return;
                }
            }
        }

        // Piirretään vaihtoehdot
        window.clear();
        window.draw(retryText);
        window.draw(quitText);
        window.display();
    }
}


void Game::resetGame() {
    // Nollataan maasto ensin
    terrain.initialize(); // Luo uusi maasto ennen tankkien sijoittamista

    // Nollataan tankkien sijainnit ja tilat alkuperäisiin sijainteihin
    tank1.reset(terrain, tank1StartPosition); // Tarkistaa uuden maaston
    tank2.reset(terrain, tank2StartPosition); // Tarkistaa uuden maaston

    // Nollataan EventManager ja muut pelitilat
    eventManager.reset(tank1, tank2, *this);
    projectiles.clear(); // Tyhjennetään ammukset

    windForce = (std::rand() % 200 - 100) / 100000.0f;
    gravity = 0.0005f;

    turnClock.restart();
    waitingForTurnSwitch = false;
}
