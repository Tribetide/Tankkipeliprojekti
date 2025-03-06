#include "EventManager.hpp"
#include <iostream>

EventManager::EventManager(Tank &t1, Tank &t2) 
    : tank1(t1), tank2(t2), currentTank(0) {}  // Pelaaja 1 aloittaa

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
            switchTurn();
        }
    }
    

void EventManager::switchTurn() {
    currentTank = (currentTank == 0) ? 1 : 0;
}

int EventManager::getCurrentTurn() const {
    return currentTank;
}
