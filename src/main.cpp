#include "../include/Game.hpp"
#include "../include/menu.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Tankkipeli");

    int choice = Menu::showMenu(window); 

    if (choice == 1) {
        Game game;
        game.run();
    }

    return 0; 
}
