#include "Terrain.hpp"
#include <cmath>

Terrain::Terrain() {}

void Terrain::initialize() {
    terrainImage.create(1920, 1080, sf::Color::Transparent); // 🔥 Taivas on läpinäkyvä

    // 🔥 Täytetään vain alaosa vihreällä (esim. alemmat 600 pikseliä)
    for (int x = 0; x < terrainImage.getSize().x; x++) {
        for (int y = 300; y < terrainImage.getSize().y; y++) { // 🔥 Muokkaa, jos haluat enemmän/vähemmän maata
            terrainImage.setPixel(x, y, sf::Color::Green);
        }
    }

    texture.loadFromImage(terrainImage);
    sprite.setTexture(texture);
    sprite.setPosition(0, 0);
}

void Terrain::draw(sf::RenderWindow &window) { // 🔥 Piirtää maaston
    window.draw(sprite);  // 🔥 Piirretään sprite
} 

bool Terrain::checkCollision(sf::Vector2f position) {
    int x = static_cast<int>(position.x);
    int y = static_cast<int>(position.y); // 🔥 Korjaa, jos maaston korkeus muuttui

    // 🔥 Varmistetaan, että koordinaatit ovat maaston rajojen sisällä
    if (x >= 0 && x < terrainImage.getSize().x && y >= 0 && y < terrainImage.getSize().y) {
        return terrainImage.getPixel(x, y).a != 0;  // Jos pikseli ei ole läpinäkyvä, se on maastoa
    }
    return false;
}


void Terrain::destroy(sf::Vector2f position, int baseRadius) {
    int x0 = static_cast<int>(position.x);
    int y0 = static_cast<int>(position.y);

    // 🔥 Tuhoutuminen riippuu siitä, kuinka syvällä ammus osuu
    int radius = baseRadius + (y0 / 20); // Mitä alempana, sitä isompi räjähdys

    for (int i = -radius; i <= radius; ++i) {
        for (int j = -radius; j <= radius; ++j) {
            if (std::sqrt(i * i + j * j) <= radius) {
                int x = x0 + i;
                int y = y0 + j;

                if (x >= 0 && x < terrainImage.getSize().x && y >= 0 && y < terrainImage.getSize().y) {
                    terrainImage.setPixel(x, y, sf::Color::Transparent);
                }
            }
        }
    }

    // 🔥 Muista päivittää tekstuuri!
    texture.update(terrainImage);
}


