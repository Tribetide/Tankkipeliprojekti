#include "EventManager.hpp"
#include <iostream>

EventManager::EventManager(Tank &t1, Tank &t2) 
    : tank1(t1), tank2(t2), currentTank(0) {}  // Pelaaja 1 aloittaa

void EventManager::handleShot(Projectile &projectile, Terrain &terrain) {
    Tank &opponent = (currentTank == 0) ? tank2 : tank1;

    // 🔥 Tarkistetaan, osuuko ammuksen sijainti tankkiin
//    if (opponent.getGlobalBounds().contains(projectile.shape.getPosition())) {
//        std::cout << "Osuma tankkiin!" << std::endl;
//        opponent.takeDamage(20); // 🔥 Tankki menettää 20 HP
//    }

    // 🔥 Vuoro vaihtuu laukauksen jälkeen
    switchTurn();
}

void EventManager::switchTurn() {
    currentTank = (currentTank == 0) ? 1 : 0;
}

int EventManager::getCurrentTurn() const {
    return currentTank;
}
