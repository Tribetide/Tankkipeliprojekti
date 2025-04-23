#include "Terrain.hpp"
#include <Game.hpp> 
#include <cmath>       // floor, fmod
#include <algorithm>   // shuffle
#include <numeric>     // iota
#include <random>
#include <cstdlib>     // srand, rand
#include <ctime>       // time
#include <iostream>    // debug prints if needed

// =============================
// Sisäinen PerlinNoise-luokka
// =============================
namespace {
    class PerlinNoise {
    public:
        explicit PerlinNoise(unsigned int seed = 0) {
            // Alustetaan taulukko [0..255]
            p.resize(256);
            std::iota(p.begin(), p.end(), 0);

            // Sekoitetaan halutulla seedillä
            std::default_random_engine engine(seed);
            std::shuffle(p.begin(), p.end(), engine);

            // Kopioidaan perään -> p.size() == 512
            p.insert(p.end(), p.begin(), p.end());
        }

        // Palauttaa arvon välillä ~[-1, 1]
        double noise(double x, double y) const {
            int xi = (int)std::floor(x) & 255;
            int yi = (int)std::floor(y) & 255;

            // hash, tämä
            double xf = x - std::floor(x);
            double yf = y - std::floor(y);

            int aa = p[p[xi] + yi];
            int ab = p[p[xi] + yi + 1];
            int ba = p[p[xi + 1] + yi];
            int bb = p[p[xi + 1] + yi + 1];

            double u = fade(xf);
            double v = fade(yf);

            double x1 = lerp(grad(aa, xf, yf), grad(ba, xf - 1, yf), u);
            double x2 = lerp(grad(ab, xf, yf - 1), grad(bb, xf - 1, yf - 1), u);
            return lerp(x1, x2, v);
        }

    private:
        double fade(double t) const {
            return t * t * t * (t * (t * 6 - 15) + 10);
        }
        double lerp(double a, double b, double t) const {
            return a + t * (b - a);
        }
        double grad(int hash, double x, double y) const {
            switch (hash & 3) {
                case 0: return  x + y;
                case 1: return -x + y;
                case 2: return  x - y;
                case 3: return -x - y;
            }
            // fallback
            return 0.0;
        }

        std::vector<int> p;  // Permutaatio-taulukko
    };


    // Kerrosvärit 
    const sf::Color GRASS_COLOR  ( 34,139, 34);  // vihreä
    const sf::Color SOIL_COLOR   (139, 69, 19);  // ruskea
    const sf::Color ROCK_COLOR   (128,128,128);  // harmaa

    // Kerrosten paksuudet pikseleinä 
    constexpr int GRASS_THICK = 30;     // pinnalla   
    constexpr int SOIL_THICK  = 250;    // ruohon alla  
                                        // loput → kallio
    
}


// =============================
// Terrain-luokan toteutus
// =============================
Terrain::Terrain() {
    std::srand(static_cast<unsigned>(std::time(nullptr))); // satunnainen siemen
}

// Alustetaan maasto Perlin noise -tekniikalla
void Terrain::initialize() {
    
    createSky(); // Kuu & tähdet pysyvät

    // Luodaan kuva aluksi tyhjänä (läpinäkyvä) koko ruudun kokoisena
    terrainImage.create(1920, 1080, sf::Color::Transparent);

    // Luodaan PerlinNoise-olio dynaamisella seedillä
    unsigned int seed = static_cast<unsigned>(std::time(nullptr));
    PerlinNoise perlin(seed);

    // Luodaan PerlinNoise-oliot eri tarkoituksiin
    PerlinNoise soilNoise(seed + 4242);   // maan siemen
    PerlinNoise grassNoise(seed + 1337);  // ruohon siemen
    PerlinNoise ditherNoise(seed + 9999); // dither noise
    

    // Parametrit, joilla voi muuttaa maaston ulkonäköä
    float scale       = 0.001f;   // x-skaala (mitä isompi, sitä "tiheämpi" maaston vaihtelu), 0.001..0.1
    int   octaves     = 3;       // montako oktaavia, mitä enemmän, sitä "sotkuisempi" maasto, 1..8
    float persistence = 0.4f;    // amplitudin pieneneminen per oktaavi, 0..1, 1 = ei pienenemistä
    int   baseLine    = 100;     // peruskorkeus, josta maasto lähtee, 0..1080, 0 on yläreuna, 1080 on alareuna
    int   maxVariation= 900;     // maksimikorkeus perlin-kukkuloille, 0..1080

    // Peruslinjan y-koordinaatti (missä maa alkaa)
    std::vector<int> groundHeights(1920);

    // Käydään 1920 pikseliä x-suunnassa
    for (int x = 0; x < 1920; x++) {
        double noiseValue = 0.0;
        double frequency  = 1.0;
        double amplitude  = 1.0;

        // Lasketaan fractal Perlin noise
        for (int o = 0; o < octaves; o++) {
            double nx = x * scale * frequency;
            double ny = 0.0;  // 1D-profiili
            double val = perlin.noise(nx, ny); // ~[-1..1]
            noiseValue += val * amplitude;

            frequency  *= 2.0;
            amplitude  *= persistence;
        }

        // Normalisoidaan noiseValue -> [0..1]
        double normalized = (noiseValue + 1.0) / 2.0;

        // Muodostetaan lopullinen y-koordinaatti
        int groundY = baseLine + static_cast<int>(normalized * maxVariation);

        // Tallennetaan groundY taulukkoon
        groundHeights[x] = groundY;

    }

    //  Piirretään pikselit
    //   Käydään uudelleen x:ssä, poimitaan groundHeights[x],
    //   lasketaan jyrkkyys (slope), ja maalataan y = groundY..1080
    for (int x = 0; x < 1920; x++) {
        int gY = groundHeights[x];
        if (gY < 0)   gY = 0;
        if (gY > 1080) gY = 1080;

      
         // Perlinen satunnainen vaihtelu ruoho‑kerroksen paksuuteen
        double ng = grassNoise.noise(x * 0.003, 0.0);
        int    localGrass = GRASS_THICK + static_cast<int>(ng * 50);
        localGrass = std::clamp(localGrass, 25, 35);

        // Perlinen satunnainen vaihtelu maa‑kerroksen paksuuteen
        double ns = soilNoise.noise(x * 0.005, 0.0);      // −1…1
        int    localSoil = SOIL_THICK + static_cast<int>(ns * 50);
        localSoil = std::clamp(localSoil, 200, 250);      // raja‑arvot


        // Lasketaan jyrkkyys verrattuna viereiseen x+1
        int slope = 0;
        if (x < 1919) {
            slope = std::abs(groundHeights[x] - groundHeights[x+1]);
        }

        // Piirretään maata groundY:stä alaspäin
        for (int y = gY; y < 1080; y++) {

            // Syvyys maa‑pinnan ylärajasta
            int depth = y - gY;

            sf::Color baseCol;
            if (depth < localGrass) {                // ruoho
                baseCol = GRASS_COLOR;
            } else if (depth < localGrass + localSoil) { // multa
                baseCol = SOIL_COLOR;
            } else {                                 // kallio
                baseCol = ROCK_COLOR;
            }

            // Perlinin satunnainen dither noise
            int dither = ((x ^ y) & 3) - 15; //
            int slopeDarken = slope;                        // jyrkkyys tummentaa

            int r = std::clamp<int>(baseCol.r + dither - slopeDarken, 0, 255);
            int g = std::clamp<int>(baseCol.g + dither - slopeDarken, 0, 255);
            int b = std::clamp<int>(baseCol.b + dither - slopeDarken, 0, 255);

            // Asetetaan pikseli
            terrainImage.setPixel(x, y, sf::Color(r,g,b));
        }
    }

    // Päivitetään tekstuuri & sprite
    texture.loadFromImage(terrainImage);
    sprite.setTexture(texture);
    sprite.setPosition(0, 0);    // Asetetaan sprite alkuperäiseen paikkaan
}

void Terrain::update(float deltaTime) {
    shootingStarTimer += deltaTime;

    if (shootingStarTimer >= 55.0f) {
        ShootingStar star;
        star.position = sf::Vector2f(std::rand() % 1920, std::rand() % 200); // Satunnainen aloituspaikka
        star.velocity = sf::Vector2f(-150.0f + (std::rand() % 100), 50.0f); // Tähdenlennon nopeus
        star.lifetime = 2.0f;

        shootingStars.push_back(star);
        shootingStarTimer = 0.0f;
    }

    for (auto &star : shootingStars) {
        star.previousPositions.push_back(star.position); 
        if (star.previousPositions.size() > 40) {
            star.previousPositions.erase(star.previousPositions.begin());
        }

        star.position += star.velocity * deltaTime;
        star.lifetime -= deltaTime;
    }

    shootingStars.erase(
        std::remove_if(shootingStars.begin(), shootingStars.end(),
                       [](const ShootingStar &s){ return s.lifetime <= 0; }),
        shootingStars.end());
}

void Terrain::createSky() {
    // Luodaan satunnaisia tähtiä
    stars.clear();
    for (int i = 0; i < 50; i++) {
        float x = std::rand() % 1920;
        float y = std::rand() % 400; // Tähdet korkeintaan 400 pikseliä ylhäältä
        stars.push_back(sf::Vector2f(x, y));
    }
}

void Terrain::draw(sf::RenderWindow &window) {
    // Piirretään tähtitaivas
    for (const auto& star : stars) {
        sf::CircleShape starShape(2);
        starShape.setFillColor(sf::Color::White);
        starShape.setPosition(star);
        window.draw(starShape);
    }

    // Piirretään tähdenlennot
    for (const auto& star : shootingStars) {
        float alphaStep = 1.0f / star.previousPositions.size();

        for (size_t i = 0; i < star.previousPositions.size(); i++) {
            sf::CircleShape tail(5 - i * 0.2f); 
            int alpha = std::max(0, static_cast<int>(255 - i * alphaStep));
            tail.setFillColor(sf::Color(255, 255, 255, alpha));
            tail.setPosition(star.previousPositions[i]);
            window.draw(tail);
        }

        // Piirretään itse "tähdenlento"
        sf::RectangleShape shootingStar(sf::Vector2f(10, 2));
        shootingStar.setFillColor(sf::Color::White);
        shootingStar.setPosition(star.position);
        shootingStar.setRotation(30.0f);
        window.draw(shootingStar);
    }

    // Piirretään varsinainen maastosprite
    window.draw(sprite);
}

bool Terrain::checkCollision(sf::Vector2f position) {
    int x = static_cast<int>(position.x);
    int y = static_cast<int>(position.y);

    if (x >= 0 && x < terrainImage.getSize().x &&
        y >= 0 && y < terrainImage.getSize().y) {
        // Palauttaa true, jos pikseli ei ole läpinäkyvä
        return (terrainImage.getPixel(x, y).a != 0);
    }
    return false;
}

// Tuhotaan pikseleitä maastosta
std::vector<PixelInfo> Terrain::destroy(sf::Vector2f position, int baseRadius) {
    int x0 = static_cast<int>(position.x);
    int y0 = static_cast<int>(position.y);

    int radius = baseRadius + (y0 / 20); // Suurempi säde alhaalla, pienempi ylhäällä

    // Tässä voisit kerätä hajotettavat pikselit talteen:
    std::vector<PixelInfo> destroyedPixelInfos;

    for (int i = -radius; i <= radius; ++i) {
        for (int j = -radius; j <= radius; ++j) {
            if (std::sqrt(i * i + j * j) <= radius) {
                int x = x0 + i;
                int y = y0 + j;
                if (x >= 0 && x < (int)terrainImage.getSize().x &&
                    y >= 0 && y < (int)terrainImage.getSize().y) 
                {
                    // Lue pikselin väri ENNEN tyhjentämistä:
                    sf::Color oldColor = terrainImage.getPixel(x, y);

                    // Jos pikseli ei ole jo läpinäkyvä, tallenna info
                    if (oldColor.a != 0) {
                        PixelInfo info;
                        info.coords = sf::Vector2i(x, y);
                        info.color  = oldColor;
                        destroyedPixelInfos.push_back(info);
                    }
                    // Tyhjennetään pikseli (läpinäkyvä)
                    terrainImage.setPixel(x, y, sf::Color::Transparent);
                }
            }
        }
    }

    // Päivitetään tekstuuri
    texture.update(terrainImage);

    // Palautetaan tuhotut pikselit
    return destroyedPixelInfos;
}
