#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include "Tank.hpp"
#include "Projectile.hpp"
#include "Terrain.hpp"
#include <SFML/System/Clock.hpp>

// 🔥 Vain eteenpäin ilmoitus Game-luokasta
class Game;



class EventManager {
public:
    EventManager(Tank &t1, Tank &t2, Game &game);  // Konstruktorissa viite Gameen
    
    void handleShot(Projectile &projectile, Terrain &terrain);
    void update(const std::vector<Projectile>& projectiles);
    void switchTurn(float &windForce, Game &game);
    int getCurrentTurn() const;
    float getTimeLeft() const;
    void stopTurnTimer();
    void restartTurnTimer();
    bool isTurnTimerPaused() const;
    bool anyProjectilesAlive(const std::vector<Projectile>& projectiles) const;

    void reset(Tank &t1, Tank &t2, Game &game);  // Reset-toteutus siirtyy .cpp-tiedostoon

    // Estetään kopiointi ja siirto eksplisiittisesti
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

private:
    Tank &tank1;
    Tank &tank2;
    int currentTank;
    sf::Clock turnClock;
    bool turnTimerPaused = false;
    float pausedTime = 0.0f;
    sf::Clock turnSwitchClock;
    bool waitingForTurnSwitch = false;
    Game &game;  // Viite Game-objektiin
};

#endif
