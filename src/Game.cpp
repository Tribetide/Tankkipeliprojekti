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
#include <SoundManager.hpp>
#include "Tank.hpp"
#include "Explosion.hpp"
#include "Terrain.hpp"


void printCurrentWorkingDirectory() {
    std::cout << "Nykyinen hakemisto: " 
              << std::filesystem::current_path() 
              << std::endl;
}

// Apufunktio, joka tarkistaa, osuuko tankki räjähdyksen vaikutusalueelle.
bool isTankInExplosionArea(const Tank &tank, const std::vector<Explosion> &explosions) {
    sf::FloatRect tankBounds = tank.getBounds();
    for (const Explosion &expl : explosions) {
        if (tankBounds.intersects(expl.getBounds())) {
            return true;
        }
    }
    return false;
}

Game::Game(sf::RenderWindow& win)
    : window(win),
    gravity(Config::GRAVITY),
    windForce(Config::getRandomWind()),
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

    // 🔥 Alustetaan satunnainen tuuli
    std::srand(std::time(nullptr));
    windForce = Config::getRandomWind();  //
}


void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        SoundManager::getInstance().update();
        render();
        tank1.mouseControlEnabled = true;
        tank2.mouseControlEnabled = true;
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

            // 🔥 Näppäimistön ESC-näppäimen käsittely
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                int result = Menu::showPauseMenu(window);
                if (result == 1) {
                    // Continue – ei tehdä mitään
                } else if (result == 2) {
                    resetGame();
                } else if (result == 3) {
                    window.close();
                }
            }
            

        // 🔥 Estetään toiminnot, jos odotetaan vuoron vaihtoa
        if (waitingForTurnSwitch) 
            return;

        // 🔥 Määritä aktiivinen tankki ja vastustaja
        Tank &activeTank = (eventManager.getCurrentTurn() == 0) ? tank1 : tank2;
        const Tank &opponentTank = (eventManager.getCurrentTurn() == 0) ? tank2 : tank1;

        // 🔥 Näppäimistön käsittely tankille
        if (event.type == sf::Event::KeyPressed) {
            activeTank.handleInput(event.key.code, terrain, projectiles, waitingForTurnSwitch, turnClock, opponentTank);
    }
        // Hiiriohjaus ohjaukseen ja tankin liikkumiseen:
        if (event.type == sf::Event::MouseMoved) {
            activeTank.handleMouseInput(window, projectiles, waitingForTurnSwitch, turnClock);
        }

        // Vasen hiirinappi = ampuminen
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) 
        {
            // Luo ja laukaise uusi projektiili
            projectiles.push_back(activeTank.shoot());
            // Nollaa vuoroajastin ja aloitetaan odotus vuoron vaihtoon
            turnClock.restart();
            waitingForTurnSwitch = true;
        }

        // 🔥 Hiiren rullakäyttö: säädetään voimaa
        if (event.type == sf::Event::MouseWheelScrolled) {
                float delta = event.mouseWheelScroll.delta;
                activeTank.adjustPower(delta * 5.0f); // Voiman säätö scrollilla
            }
        
    }
}

void Game::update() {
    float deltaTime = 0.9f / 60.0f;

    // 🔥 Päivitä räjähdykset ensin
    for (auto &e : explosions) {
        e.update(deltaTime);
    }
    explosions.erase( // 🔥 Poista räjähdykset, jotka ovat valmiita
        std::remove_if(explosions.begin(), explosions.end(), 
                        [](const Explosion &e) { return e.isFinished(); }),
        explosions.end()
    );
   
    // Päivitä tankit
    auto updateTankWithExplosionCheck = [&](Tank& t) {
        if (!isTankInExplosionArea(t, explosions)) {
            t.update(terrain, gravity);
        }
    };

    updateTankWithExplosionCheck(tank1);
    updateTankWithExplosionCheck(tank2);


    // 🔥 Määritä aktiivinen tankki sekä vastustajan
    Tank &activeTank   = (eventManager.getCurrentTurn() == 0) ? tank1 : tank2;
    Tank &opponentTank = (eventManager.getCurrentTurn() == 0) ? tank2 : tank1;

    // 🔥 Päivitä aktiivinen tankki
    activeTank.update(terrain, gravity); 

    // 🔥 Päivitä kaikki ammukset
    
    terrain.update(deltaTime); // 🔥 Päivitetään tähdenlennot

    for (auto &proj : projectiles) { // 🔥 Ammusten päivitys
        // Päivitetään ammuksen liike
        proj.update(deltaTime, terrain, windForce);
    
        // Tarkistetaan, osuuko ammuksen rajat vastustajan tankin rajojen kanssa
        if (proj.alive && proj.getBounds().intersects(opponentTank.getBounds())) {
            opponentTank.takeDamage(30);
            explosions.emplace_back(proj.shape.getPosition());
            proj.alive = false;
            std::vector<PixelInfo> destroyed = terrain.destroy(proj.shape.getPosition(), 60);
            spawnDebris(destroyed, proj.shape.getPosition()); // 60:lla määritellään laajempi tuhoamisalue
            SoundManager::getInstance().playSound("explosion", 100.f);
        }
    
        // Jos ammus osuu myös maahan
        if (proj.alive && terrain.checkCollision(proj.shape.getPosition())) {
            explosions.emplace_back(proj.shape.getPosition());
            proj.alive = false;
            std::vector<PixelInfo> destroyed = terrain.destroy(proj.shape.getPosition(), 50);
            spawnDebris(destroyed, proj.shape.getPosition());
            SoundManager::getInstance().playSound("explosion", 100.f);
            std::cout << "Räjähdyksiä aktiivisena: " << explosions.size() << std::endl;
        }
    
        if (proj.alive && terrain.checkCollision(proj.shape.getPosition())) {
            explosions.emplace_back(proj.shape.getPosition()); // 🔥 Lisää räjähdys
            proj.alive = false;
            std::vector<PixelInfo> destroyed = terrain.destroy(proj.shape.getPosition(), 50);
            spawnDebris(destroyed, proj.shape.getPosition());
            SoundManager::getInstance().playSound("explosion", 100.f);
            std::cout << "Räjähdyksiä aktiivisena: " << explosions.size() << std::endl;

        }
    }

    // 🔥 Poista kuolleet ammukset listasta
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile &p) {
            return !p.alive;
        }),
        projectiles.end()
    );

    // 🔥 Päivitä eventManager ja anna sille projektiililista
    eventManager.update(projectiles);

    if (waitingForTurnSwitch) { // 🔥 Odotetaan vuoron vaihtoa
        if (turnClock.getElapsedTime().asSeconds() >= 2.0f && !eventManager.anyProjectilesAlive(projectiles)) {
            eventManager.switchTurn(windForce, *this);  // 🔥 Lisätty `*this`
            waitingForTurnSwitch = false;
            eventManager.restartTurnTimer();
        }
    }


    // Päivitetään partikkelien sijainti ja tarkistetaan törmäykset maastoon
    for (auto &d : debrisList) {
        // 1) Liikutetaan partikkelia
        d.position += d.velocity * deltaTime;
        
        // 2) Painovoima
        d.velocity.y += gravity * 0.5f * deltaTime;
        
        // 3) Tarkistetaan törmääkö maahan
        if (d.position.x >= 0 && d.position.x < 1920 &&
            d.position.y >= 0 && d.position.y < 1080)
        {
            // Jos pikseli on "kiinteä" (maata) => pysäytä velocity
            if (terrain.checkCollision(d.position)) {
                // Yksinkertaisesti nostetaan partikkelia takaisin 1px,
                // jotta se ei uppoa maahan
                d.position.y -= 1.0f;
                // Pysäytetään velocity
                d.velocity = sf::Vector2f(0.f, 0.f);
            }
        }
        // 4) Vähennetään elinaikaa
        d.lifetime -= deltaTime;
    }

        // Poista kuolleet partikkelit
        debrisList.erase(
            std::remove_if(debrisList.begin(), debrisList.end(),
                [](const Debris &d){ return d.lifetime <= 0; }),
            debrisList.end()
        );
}


void Game::render() {
    window.clear();
    window.draw(moonSprite);
    terrain.draw(window);

    float totalTime = globalClock.getElapsedTime().asSeconds();

    // 🔥 Piirrä kaikki räjähdysefektit
    for (const auto& e : explosions) {
        e.draw(window);
    }

    // Piirretään tankit
    tank1.draw(window);
    tank2.draw(window);

    // Piiretään ammukset
    for (auto &p : projectiles) {
        if (p.alive) window.draw(p.shape);
    }

    //Piirreään debris-partikkelit
    for (auto &d : debrisList) {
        // Valitse jokin piirrettävä muoto (esim. pieni ympyrä).
        sf::CircleShape shape(3.f); 
        shape.setPosition(d.position);
        shape.setFillColor(d.color);
        window.draw(shape);
    }

    // Hae vuorossa oleva tankki
    Tank &currentTank = (eventManager.getCurrentTurn() == 0) ? tank1 : tank2;

    if (!waitingForTurnSwitch) {
        sf::Vector2f aimPos = currentTank.getAimPoint(); // 500 px
        UI::drawCrosshair(window, aimPos);
    }

    // ristiä ei  näy ammuksen lentäessä
    if (!waitingForTurnSwitch && !eventManager.anyProjectilesAlive(projectiles)) {
        UI::drawCrosshair(window, currentTank.getAimPoint());
    }

    // Kutsutaan UI-piirtämisen funktioita
    UI::drawTurnText(window, font, eventManager);
    UI::drawTurnTimer(window, font, eventManager);    
    UI::drawAngleText(window, font, currentTank);
    UI::drawPowerText(window, font, currentTank);
    UI::drawWindText(window, font, windForce);
    UI::drawWindBarIndicator(window, windForce);
    UI::drawTankHp(window, font, currentTank); 
    UI::drawFuelMeter(window, font, currentTank);

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(800, 20);
    scoreText.setString("T1 Wins: " + std::to_string(tank1Wins) +
                        "   T2 Wins: " + std::to_string(tank2Wins));
    window.draw(scoreText);
    

    window.display();
}

void Game::endGame() {
    // Näytetään voittoteksti
    sf::Text winnerText;
    winnerText.setFont(font);
    winnerText.setCharacterSize(50);
    winnerText.setFillColor(sf::Color::Green);

    if (tank1.getHp() == 0 && tank2.getHp() > 0) {
        tank2Wins++;
        winnerText.setString("Tank 2 wins!");
    } else if (tank2.getHp() == 0 && tank1.getHp() > 0) {
        tank1Wins++;
        winnerText.setString("Tank 1 wins!");
    } else {
        draws++;
        winnerText.setString("It's a draw!");
    }
    

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
    sf::Text totalScore("T1: " + std::to_string(tank1Wins) + 
                    " | T2: " + std::to_string(tank2Wins) +
                    " | Draws: " + std::to_string(draws), font, 30);
totalScore.setFillColor(sf::Color::White);
totalScore.setPosition(200, 280);
window.draw(totalScore);

    window.display();

    // Pidetään voittoteksti näkyvillä 3 sekuntia
    sf::sleep(sf::seconds(3));

    // Siirrytään vaihtoehtoihin
    sf::Text retryText("1. Retry", font, 30);
    retryText.setPosition(800, 500);
    retryText.setFillColor(sf::Color::Green);

    sf::Text quitText("2. Quit", font, 30);
    quitText.setPosition(800, 500);
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

void Game::spawnDebris(const std::vector<PixelInfo>& destroyedPixels, sf::Vector2f center) {
    // esim. rajoitetaan partikkelien maksimimäärää
    int maxParticles = 100;  
    int count = 0;

    for (auto &pixInfo : destroyedPixels) {
        if (count >= maxParticles) break;

        // Sijainti float-muodossa:
        sf::Vector2f pos((float)pixInfo.coords.x, (float)pixInfo.coords.y);

        // Haetaan pikselille satunnainen nopeus
        // (pienet hajonnat, esim. -100..100 x, -300.. -100 y)
        float vx = -100 + std::rand() % 201;   // 
        float vy = -100 + std::rand() % 201;  // 
        sf::Vector2f vel(vx, vy);

        // Väri (voit tallettaa col suoraan, jos tallensit sen jo)
        sf::Color col = pixInfo.color;  // esim. vihreä
        // Tai anna random tummuus-sävy:
        // col.r = 0; col.g = 128 + (std::rand()%128); col.b = 0; col.a = 255;

        // Partikkelin elinaika
        float life = 9.0f + (std::rand() % 20) / 10.f;

        debrisList.emplace_back(pos, vel, col, life);
        count++;
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

    gravity = Config::GRAVITY;
    windForce = Config::getRandomWind();

    turnClock.restart();
    waitingForTurnSwitch = false;
}
