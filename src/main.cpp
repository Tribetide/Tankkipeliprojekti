#include "../include/Game.hpp"
#include "../include/menu.hpp"
#include <iostream>
#include "SoundManager.hpp"

int main() {
    // 1. Ikkuna
    sf::RenderWindow window(sf::VideoMode(800, 600), "Tankkipeli");

    // 2. Lataa äänitiedostot
    auto& soundMgr = SoundManager::getInstance();
    if (!soundMgr.loadSound("fire", "assets/sounds/Fire.ogg")) {
        std::cerr << "Virhe ladattaessa Fire.ogg\n";
    }
    if (!soundMgr.loadSound("explosion", "assets/sounds/Explosion.mp3")) {
        std::cerr << "Virhe ladattaessa Explosion.mp3\n";
    }
    if (!soundMgr.loadMusic("war", "assets/sounds/War.ogg")) {
        std::cerr << "Virhe ladattaessa War.ogg\n";
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
        Game game;
        game.run(); 
        // Huom: Game::run()-silmukassa (tai muualla) tulee kutsua
        // SoundManager::getInstance().update();
        // jokaisella ruudunpäivityksellä.
    }

    return 0;
}
