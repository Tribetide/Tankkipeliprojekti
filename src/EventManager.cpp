#include "EventManager.hpp"
#include <iostream>
#include <cstdlib>   // 🔥 Satunnaislukujen generointiin
#include <ctime>     // 🔥 Aikasiemen satunnaislukujen alustamiseen
#include <cmath>
#include "Game.hpp"
#include <Config.hpp>

float EventManager::getTimeLeft() const {
    // 🔥 Jos ajastin on pysäytetty, näytetään jäljellä oleva aika tallennetusta arvosta
    float timeLeft = TURN_TIME_LIMIT - (turnTimerPaused ? pausedTime : turnClock.getElapsedTime().asSeconds());
    return std::max(0, static_cast<int>(std::floor(timeLeft)));
}

EventManager::EventManager(Tank &t1, Tank &t2, Game &g) 
    : tank1(t1), tank2(t2), game(g) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));  // 🔥 Alustetaan satunnaislukugeneraattori
    currentTank = std::rand() % 2;  // 🔥 Arvotaan 0 (tank1) tai 1 (tank2)
    turnClock.restart(); // 🔥 Aloitetaan vuoroajastin
}

    void EventManager::reset(Tank &t1, Tank &t2, Game & /*game*/) {
        tank1 = t1;
        tank2 = t2;
        currentTank = 0;
        turnClock.restart();
        turnTimerPaused = false;
        pausedTime = 0.0f;
        waitingForTurnSwitch = false;
    }


void EventManager::update(const std::vector<Projectile>& projectiles) {
    if (waitingForTurnSwitch) {
        if (turnSwitchClock.getElapsedTime().asSeconds() >= 2.0f) {  
            float windForce = 0.0f;  // 
            switchTurn(windForce, game);
            waitingForTurnSwitch = false;
        }
    } 
    // 🔥 Ajastin on mennyt nollaan, mutta tarkistetaan, onko ammus elossa
    else if (!turnTimerPaused && turnClock.getElapsedTime().asSeconds() > TURN_TIME_LIMIT) {
        if (!anyProjectilesAlive(projectiles)) {  
            float windForce = 0.0f;
            switchTurn(windForce, game);
        } 
    }
}



void EventManager::switchTurn(float &windForce, Game &game) {
    // 🔥 Tarkistetaan ennen vuoron vaihtoa, onko peli päättynyt
    if (tank1.getHp() == 0 || tank2.getHp() == 0) {
        game.endGame();  // 🔥 Jos joku tankki on kuollut, lopetetaan peli
        return;
    }

    // Jos peli ei ole vielä päättynyt, vaihdetaan vuoro
    currentTank = (currentTank == 0) ? 1 : 0;
    turnClock.restart();
    windForce = Config::getRandomWind();  // 🔥 Arvotaan uusi tuuli

    //  Resetoi polttoaine uuden vuoron alkaessa
    if (currentTank == 0) {
        tank1.resetFuel();
    } else {
        tank2.resetFuel();
    }
}


int EventManager::getCurrentTurn() const {
    return currentTank;
}

void EventManager::stopTurnTimer() {
    if (!turnTimerPaused) {
        pausedTime = turnClock.getElapsedTime().asSeconds();  // 🔥 Tallennetaan kulunut aika
        turnTimerPaused = true;
    }
}

void EventManager::restartTurnTimer() {
    turnClock.restart();
    turnTimerPaused = false;  // 🔥 Nollataan tauko
}

bool EventManager::isTurnTimerPaused() const {
    return turnTimerPaused;
}

bool EventManager::anyProjectilesAlive(const std::vector<Projectile>& projectiles) const {
    for (const auto &p : projectiles) {
        if (p.alive) return true;
    }
    return false;
}
