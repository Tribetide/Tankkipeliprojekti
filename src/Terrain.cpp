#include "Terrain.hpp"
#include <cmath>

Terrain::Terrain() {}

#include <cstdlib>
#include <ctime>


void Terrain::initialize() {
    std::srand(std::time(nullptr)); // Satunnaislukugeneraattorin alustus
    createSky(); // Luodaan kuu ja tähdet
    terrainImage.create(1920, 1080, sf::Color::Transparent);

    int groundHeight[1920]; // Taulukko maaston korkeuksille

    float frequency1 = 0.003f + static_cast<float>(std::rand() % 3) / 1000.0f; // Pääaallonpituus
    float frequency2 = 0.009f + static_cast<float>(std::rand() % 5) / 1000.0f; // Toissijainen aallonpituus
    float amplitude1 = 100.0f + std::rand() % 50; // Ensimmäinen aalto
    float amplitude2 = 50.0f + std::rand() % 30; // Toinen aalto
    int baseHeight = 500 + std::rand() % 150; // Peruskorkeus satunnaistettuna

    for (int x = 0; x < terrainImage.getSize().x; x++) {
        int randomOffset = std::rand() % 5 - 3; // Lisää pientä vaihtelua
        groundHeight[x] = baseHeight +
                          std::sin(x * frequency1) * amplitude1 +
                          std::sin(x * frequency2) * amplitude2 +
                          randomOffset;
    }

    // **Lisätään satunnaisia kukkuloita ja kuoppia**
    for (int i = 0; i < 5; i++) { // 5 satunnaista kukkulaa
        int hillX = std::rand() % 1920;
        int hillSize = 50 + std::rand() % 100;
        int hillHeight = 20 + std::rand() % 50;

        for (int x = hillX - hillSize; x < hillX + hillSize; x++) {
            if (x >= 0 && x < 1920) {
                groundHeight[x] -= hillHeight * std::exp(-((x - hillX) * (x - hillX)) / (2.0 * hillSize * hillSize));
            }
        }
    }

    // **Täytetään maasto vihreällä**
    for (int x = 0; x < terrainImage.getSize().x; x++) {
        for (int y = groundHeight[x]; y < terrainImage.getSize().y; y++) {
            terrainImage.setPixel(x, y, sf::Color::Green);
        }
    }

    texture.loadFromImage(terrainImage);
    sprite.setTexture(texture);
    sprite.setPosition(0, 0);
}

void Terrain::update(float deltaTime) {
    shootingStarTimer += deltaTime;

    if (shootingStarTimer >= 55.0f) {
        ShootingStar star;
        star.position = sf::Vector2f(std::rand() % 1920, std::rand() % 200); // Satunnainen aloituspaikka
        star.velocity = sf::Vector2f(-150.0f + (std::rand() % 100), 50.0f); // 🔥 Hidastettu nopeus!
        star.lifetime = 2.0f;

        shootingStars.push_back(star);
        shootingStarTimer = 0.0f;
    }

    for (auto &star : shootingStars) {
        star.previousPositions.push_back(star.position); // 🔥 Talletetaan vanha sijainti häntää varten
        if (star.previousPositions.size() > 40) { // Rajataan häntä 10 osaan
            star.previousPositions.erase(star.previousPositions.begin());
        }

        star.position += star.velocity * deltaTime;
        star.lifetime -= deltaTime;
    }

    shootingStars.erase(
        std::remove_if(shootingStars.begin(), shootingStars.end(),
                       [](const ShootingStar &s) { return s.lifetime <= 0; }),
        shootingStars.end());
}



void Terrain::createSky() {

    // ⭐ Luodaan satunnaisia tähtiä
    stars.clear();
    for (int i = 0; i < 50; i++) {
        float x = std::rand() % 1920;
        float y = std::rand() % 400; // ⭐ Tähtien korkeus
        stars.push_back(sf::Vector2f(x, y));
    }
}

void Terrain::draw(sf::RenderWindow &window) { // 🔥 Piirtää maaston

    for (const auto& star : stars) {
        sf::CircleShape starShape(2);
        starShape.setFillColor(sf::Color::White);
        starShape.setPosition(star);
        window.draw(starShape);
    }


    // 🌠 Piirretään tähdenlennot ja niiden hännät
    for (const auto& star : shootingStars) {
        float alphaStep = 1.0f / star.previousPositions.size(); // 🔥 Parempi haalistuminen

        for (size_t i = 0; i < star.previousPositions.size(); i++) {
            sf::CircleShape tail(5 - i * 0.2f); // 🔥 Pienenevä häntä
            int alpha = std::max(0, static_cast<int>(255 - i * alphaStep)); // 🔥 Pehmeä haalistuminen
            tail.setFillColor(sf::Color(255, 255, 255, alpha));
            tail.setPosition(star.previousPositions[i]);
            window.draw(tail);
        }

        sf::RectangleShape shootingStar(sf::Vector2f(10, 2));
        shootingStar.setFillColor(sf::Color::White);
        shootingStar.setPosition(star.position);
        shootingStar.setRotation(30.0f);
        window.draw(shootingStar);
    }

    
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

