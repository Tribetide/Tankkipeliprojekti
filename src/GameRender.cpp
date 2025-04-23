
#include "Game.hpp"
#include "UI.hpp"
#include <filesystem> // C++17 tiedostopoluille
#include <cmath>  // Matematiikkakirjasto
#include "menu.hpp"


void Game::render() {
    window.clear();
    window.draw(moonSprite);
    terrain.draw(window);

    float totalTime = globalClock.getElapsedTime().asSeconds();

    // 🔥 Piirrä kaikki räjähdysefektit
    for (const auto& e : explosions) {
        e.draw(window);
    }

    // Piirretään vain elossa olevat tankit
    if (!tank1.isDestroyed())
        tank1.draw(window);
    if (!tank2.isDestroyed())
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
        if (!currentTank.isDestroyed()) {
                sf::Vector2f aimPos = currentTank.getAimPoint(); // 500 px
                UI::drawCrosshair(window, aimPos);
            }
    }

    // ristiä ei  näy ammuksen lentäessä
    if (!waitingForTurnSwitch && !eventManager.anyProjectilesAlive(projectiles)) {
        UI::drawCrosshair(window, currentTank.getAimPoint());
    }

    // Kutsutaan UI-piirtämisen funktioita
    UI::drawTurnText(window, font, eventManager);
    UI::drawTurnTimer(window, font, eventManager);   
    UI::drawTurnLabelOverTank(window, font, eventManager, tank1, tank2);
    UI::drawAngleText(window, font, currentTank);
    UI::drawPowerText(window, font, currentTank);
    UI::drawWindText(window, font, windForce);
    UI::drawWindBarIndicator(window, windForce);
    // Näytä tankin elinvoima ja polttoaine vain, jos tankki ei ole tuhottu
    if (!currentTank.isDestroyed()) {
            UI::drawTankHp(window, font, currentTank);
            UI::drawFuelMeter(window, font, currentTank);
        }

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(800, 20);
    scoreText.setString("Tank 1 Wins: " + std::to_string(tank1Wins) +
                        "   Tank 2 Wins: " + std::to_string(tank2Wins));
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
    sf::Text totalScore("Tank 1: " + std::to_string(tank1Wins) + 
                    " | Tank 2: " + std::to_string(tank2Wins) +
                    " | Draws: " + std::to_string(draws), font, 30);
    totalScore.setFillColor(sf::Color::White);
    totalScore.setPosition(200, 280);
    window.draw(totalScore);

    window.display();

    // Pidetään voittoteksti näkyvillä 3 sekuntia
    sf::sleep(sf::seconds(3));

    // Siirrytään vaihtoehtoihin
    sf::Text retryText("1. Retry", font, 45);
    retryText.setPosition(800, 400);
    retryText.setFillColor(sf::Color::Green);

    sf::Text quitText("2. Quit", font, 45);
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



