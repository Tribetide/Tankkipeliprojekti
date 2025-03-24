#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include "Tank.hpp"
#include "Projectile.hpp"
#include "Terrain.hpp"
#include <SFML/System/Clock.hpp>

const float TURN_TIME_LIMIT = 30.0f;

class EventManager {
public:
    EventManager(Tank &t1, Tank &t2);
    
    void handleShot(Projectile &projectile, Terrain &terrain);
    void update(const std::vector<Projectile>& projectiles);
    void switchTurn(float &windForce);
    int getCurrentTurn() const;
    float getTimeLeft() const;
    void stopTurnTimer();
    void restartTurnTimer();
    bool isTurnTimerPaused() const;
    bool anyProjectilesAlive(const std::vector<Projectile>& projectiles) const;


private:
    Tank &tank1;
    Tank &tank2;
    int currentTank;
    sf::Clock turnClock;
    bool turnTimerPaused = false;
    float pausedTime = 0.0f;  // 🔥 Tallennetaan, kuinka kauan ajastin on ollut käynnissä
    sf::Clock turnSwitchClock;  // 🔥 Lisätään viivekello vuoronvaihtoon
    bool waitingForTurnSwitch = false;  // 🔥 Ollaanko odottamassa vuoronvaihtoa
};

#endif
