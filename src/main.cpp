#include "../include/Game.hpp"
#include "../include/menu.hpp"
#include <iostream>
#include "SoundManager.hpp"

int main() {
    // 1. Ikkuna 
//    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    // Luodaan ikkuna koko näytölle
//    sf::RenderWindow window(desktop, "Tankkipeli", sf::Style::Fullscreen);

    sf::VideoMode windowMode(1920, 1080);
    sf::RenderWindow window(windowMode, "Tankkipeli", sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);

    // 2. Lataa äänitiedostot
    auto& soundMgr = SoundManager::getInstance();
    if (!soundMgr.loadSound("fire", "assets/sounds/Fire.ogg") &&
        !soundMgr.loadSound("fire", "../assets/sounds/Fire.ogg")) {
        std::cerr << "Virhe ladattaessa Fire.ogg\n";
    }
    if (!soundMgr.loadSound("explosion", "assets/sounds/Explosion.ogg") &&
        !soundMgr.loadSound("explosion", "../assets/sounds/Explosion.ogg")) {
        std::cerr << "Virhe ladattaessa Explosion.ogg\n";
    }
    if (!soundMgr.loadMusic("war", "assets/sounds/War.ogg") &&
        !soundMgr.loadMusic("war", "../assets/sounds/War.ogg")) {
        std::cerr << "Virhe ladattaessa War.ogg\n";
    }
    if (!soundMgr.loadSound("tank_destroyed", "assets/sounds/Tank_Destroyed.ogg") &&
        !soundMgr.loadSound("tank_destroyed", "../assets/sounds/Tank_Destroyed.ogg")) {
        std::cerr << "Virhe ladattaessa Tank_Destroyed.ogg\n";
    }
    

    // 3. Soita War.ogg (30% volume, toistuu loopissa)
    sf::Music* warMusic = soundMgr.getMusic("war");
    if (warMusic) {
        warMusic->setLoop(true);
        warMusic->setVolume(30.f); // Soi hiljaisemmalla
        warMusic->play();
    }

    // 4. Näytä menu
    int choice = Menu::showMenu(window);
    if (choice == 1) {
        // 5. Käynnistä peli
        Game game(window);
        game.run(); 
 
    }

    return 0;
}
