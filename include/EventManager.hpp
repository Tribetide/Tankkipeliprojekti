#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include "Tank.hpp"
#include "Projectile.hpp"
#include "Terrain.hpp"

class EventManager {
public:
    EventManager(Tank &t1, Tank &t2);
    
    void handleShot(Projectile &projectile, Terrain &terrain);
    void switchTurn(); 
    int getCurrentTurn() const;

private:
    Tank &tank1;
    Tank &tank2;
    int currentTank;
};

#endif
