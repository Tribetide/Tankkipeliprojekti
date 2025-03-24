#include "EventManager.hpp"
#include <iostream>
#include <cstdlib>   // 🔥 Satunnaislukujen generointiin
#include <ctime>     // 🔥 Aikasiemen satunnaislukujen alustamiseen
#include <cmath>

float EventManager::getTimeLeft() const {
    // 🔥 Jos ajastin on pysäytetty, näytetään jäljellä oleva aika tallennetusta arvosta
    float timeLeft = TURN_TIME_LIMIT - (turnTimerPaused ? pausedTime : turnClock.getElapsedTime().asSeconds());
    return std::max(0, static_cast<int>(std::floor(timeLeft)));
}

EventManager::EventManager(Tank &t1, Tank &t2) 
    : tank1(t1), tank2(t2) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));  // 🔥 Alustetaan satunnaislukugeneraattori
    currentTank = std::rand() % 2;  // 🔥 Arvotaan 0 (tank1) tai 1 (tank2)
    turnClock.restart(); // 🔥 Aloitetaan vuoroajastin
}

    void EventManager::handleShot(Projectile &projectile, Terrain &terrain) {
        if (!projectile.alive) return;  // 🔥 Jos ammus on jo "kuollut", älä käsittele uudelleen
    
        Tank &opponent = (currentTank == 0) ? tank2 : tank1;
    
        // 🔥 Tarkistetaan, osuuko ammuksen sijainti tankkiin
      //  if (opponent.getGlobalBounds().contains(projectile.shape.getPosition())) {
      //      std::cout << "Osuma tankkiin!" << std::endl;
      //      opponent.takeDamage(20);
      //      projectile.alive = false;  // 🔥 Ammus kuolee osuessaan
      //  }
    
        // 🔥 Tarkistetaan, onko ammus osunut maahan
        if (terrain.checkCollision(projectile.shape.getPosition())) {
            projectile.alive = false;  // 🔥 Ammus kuolee osuessaan maahan
        }
    
        // 🔥 Jos ammus kuoli nyt, vaihda vuoro
        if (!projectile.alive) {
            waitingForTurnSwitch = true;  // 🔥 Odotetaan ennen vuoron vaihtoa
            turnSwitchClock.restart();  // 🔥 Käynnistetään viivekello
        }
    }

void EventManager::update(const std::vector<Projectile>& projectiles) {
    if (waitingForTurnSwitch) {
        if (turnSwitchClock.getElapsedTime().asSeconds() >= 2.0f) {  
            float windForce = 0.0f;  // Initialize windForce
            switchTurn(windForce);
            waitingForTurnSwitch = false;
        }
    } 
    // 🔥 Ajastin on mennyt nollaan, mutta tarkistetaan, onko ammus elossa
    else if (!turnTimerPaused && turnClock.getElapsedTime().asSeconds() > TURN_TIME_LIMIT) {
        if (!anyProjectilesAlive(projectiles)) {  
            float windForce = 0.0f;  // Initialize windForce
            switchTurn(windForce);
        } 
        // 🔥 Jos ammus on yhä ilmassa, ei tehdä mitään – odotetaan sen putoamista
    }
}



void EventManager::switchTurn(float &windForce) {
    currentTank = (currentTank == 0) ? 1 : 0;
    turnClock.restart();
    windForce = (std::rand() % 100 - 50) / 100000.0f;  // 🔥 Arvotaan uusi tuuli
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
