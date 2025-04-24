
#include "Game.hpp"
#include <Config.hpp>
#include <cstdlib>  // satunnaislukujen generointi
#include <ctime>    // ajan käyttö satunnaislukujen generointiin
#include <filesystem> // C++17 tiedostopoluille
#include <cmath>  // Matematiikkakirjasto
#include "menu.hpp"
#include <SoundManager.hpp>



/*============================
    APUFUNKTIOT
==============================*/

// Tarkistetaan osuuko tankki räjähdyksen vaikutusalueelle.
bool isTankInExplosionArea(const Tank &tank, const std::vector<Explosion> &explosions) {
    sf::FloatRect tankBounds = tank.getBounds();
    for (const Explosion &expl : explosions) {
        if (tankBounds.intersects(expl.getBounds())) {
            return true;
        }
    }
    return false;
}


/*===================================
    GAME-KONSTRUKTORI & RUN-LOOP
====================================*/
Game::Game(sf::RenderWindow& win)
    : window(win),
    gravity(Config::GRAVITY),
    windForce(Config::getRandomWind()),
    eventManager(tank1, tank2, *this),
    tank1StartPosition(100, 0), // Alkuperäinen sijainti tankille 1
    tank2StartPosition(1800, 0) // Alkuperäinen sijainti tankille 2

{
    // Alustetaan maasto ja tankit
    terrain.initialize();
    tank1.placeOnTerrain(terrain, tank1StartPosition.x); // Käytetään tank1:n aloitussijaintia
    tank2.placeOnTerrain(terrain, tank2StartPosition.x); // Käytetään tank2
   

    // Kuun tekstuuri
    moonTexture.loadFromFile("assets/images/moon.png");
    moonSprite.setTexture(moonTexture);
    moonSprite.setPosition(1600, 100); // Säädä sijaintia
    moonSprite.setScale(0.6f, 0.6f);   // Säädä kokoa
    


    // fontti
    for (auto path :
        {Config::FONT_PATH_1,
         Config::FONT_PATH_2,
         Config::FONT_PATH_3})
       if (font.loadFromFile(path)) break; // Lataa fontti, jos ei onnistu, kokeile seuraavaa, 

    // 🔥 Alustetaan satunnainen tuuli
    std::srand(std::time(nullptr));
    windForce = Config::getRandomWind();  //
}


void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        SoundManager::getInstance().update();
        render();
        tank1.mouseControlEnabled = true;
        tank2.mouseControlEnabled = true;
    }
}

/*=======================================
    INPUT-KÄSITTELY
========================================*/
void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

            // 🔥 Näppäimistön ESC-näppäimen käsittely
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                int result = Menu::showPauseMenu(window);
                if (result == 1) {
                } else if (result == 2) {
                    resetGame();
                } else if (result == 3) {
                    window.close();
                }
            }
            
        // 🔥 Estetään toiminnot, jos odotetaan vuoron vaihtoa
        if (waitingForTurnSwitch) 
            return;

        // 🔥 Määritä aktiivinen tankki ja vastustaja
        Tank &activeTank = (eventManager.getCurrentTurn() == 0) ? tank1 : tank2;
        const Tank &opponentTank = (eventManager.getCurrentTurn() == 0) ? tank2 : tank1;


        /* ----------  Näppäimistö ---------- */

        // 🔥 Näppäimistön käsittely tankille
        if (event.type == sf::Event::KeyPressed) {
        // K-painallus tuhoaa tankin ja lopettaa pelin heti
        if (event.key.code == sf::Keyboard::K) {
                activeTank.takeDamage(activeTank.getHp());
                explosions.emplace_back(activeTank.getPosition());
                // Estä pelilogiikka, ja määrittele lopetustauko
                pendingEndGame = true;
                endGameClock.restart();
                return;
            }
        // 🔥 Muut näppäimet käsitellään normaalisti
            activeTank.handleInput(event.key.code, terrain, projectiles, waitingForTurnSwitch, turnClock, opponentTank);
        }
        

        /* ----------  Hiiri ---------- */


        // Hiiriohjaus ohjaukseen ja tankin liikkumiseen:
        if (event.type == sf::Event::MouseMoved) {
            activeTank.handleMouseInput(window, projectiles, waitingForTurnSwitch, turnClock);
        }

        // Vasen hiirinappi = ampuminen
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) 
        {
            // Luo ja laukaise uusi projektiili
            projectiles.push_back(activeTank.shoot());
            // Nollaa vuoroajastin ja aloitetaan odotus vuoron vaihtoon
            turnClock.restart();
            waitingForTurnSwitch = true;
        }

        // 🔥 Hiiren rullakäyttö: säädetään voimaa
        if (event.type == sf::Event::MouseWheelScrolled) 
        {
            float delta = event.mouseWheelScroll.delta;
            activeTank.adjustPower(delta * 5.0f); // Voiman säätö scrollilla
        }
        
    }
}

/* =================================================
   PELILOGIIKAN PÄIVITYS
   ================================================= */

void Game::update() {
    float deltaTime = 0.9f / 60.0f;

    //  Jos odotetaan lopetustaukoa, päivitä vain räjähdykset ja tarkista kello
    if (pendingEndGame) {
            // Päivitä räjähdyseffektit
            for (auto &e : explosions) e.update(deltaTime);
            // Poista valmiit räjähdykset (jos haluat)
            explosions.erase(
                std::remove_if(explosions.begin(), explosions.end(),
                               [](const Explosion &e){ return e.isFinished(); }),
                explosions.end());
    
            // Kun tauko on kulunut, lopeta peli
            if (endGameClock.getElapsedTime().asSeconds() >= 1.0f) {
                pendingEndGame = false;
                endGame();
            }
            return;  // älä suorita muuta logiikkaa
        }

    // 🔥 Päivitä räjähdykset ensin
    for (auto &e : explosions) {
        e.update(deltaTime);
    }
    explosions.erase( // 🔥 Poista räjähdykset, jotka ovat valmiita
        std::remove_if(explosions.begin(), explosions.end(), 
                        [](const Explosion &e) { return e.isFinished(); }),
        explosions.end()
    );
   
    // Päivitä tankit
    auto updateTankWithExplosionCheck = [&](Tank& t) {
        if (!isTankInExplosionArea(t, explosions)) {
            t.update(terrain, gravity);
        }
    };

    updateTankWithExplosionCheck(tank1);
    updateTankWithExplosionCheck(tank2);


    // 🔥 Määritä aktiivinen tankki sekä vastustajan
    Tank &activeTank   = (eventManager.getCurrentTurn() == 0) ? tank1 : tank2;
    Tank &opponentTank = (eventManager.getCurrentTurn() == 0) ? tank2 : tank1;

    // 🔥 Päivitä aktiivinen tankki
    activeTank.update(terrain, gravity); 

    // 🔥 Päivitä kaikki ammukset
    
    terrain.update(deltaTime); // 🔥 Päivitetään tähdenlennot

    for (auto &proj : projectiles) { // 🔥 Ammusten päivitys
        // Päivitetään ammuksen liike
        proj.update(deltaTime, terrain, windForce);
    
        // Tarkistetaan, osuuko ammuksen rajat vastustajan tankin rajojen kanssa
        if (proj.alive && proj.getBounds().intersects(opponentTank.getBounds())) {
            opponentTank.takeDamage(30);
            explosions.emplace_back(proj.shape.getPosition());
            proj.alive = false;
            std::vector<PixelInfo> destroyed = terrain.destroy(proj.shape.getPosition(), 60);
            spawnDebris(destroyed, proj.shape.getPosition()); // 60:lla määritellään laajempi tuhoamisalue
            SoundManager::getInstance().playSound("explosion", 100.f);
        }
    
        // Jos ammus osuu myös maahan
        if (proj.alive && terrain.checkCollision(proj.shape.getPosition())) {
            explosions.emplace_back(proj.shape.getPosition());
            proj.alive = false;
            std::vector<PixelInfo> destroyed = terrain.destroy(proj.shape.getPosition(), 50);
            spawnDebris(destroyed, proj.shape.getPosition());
            SoundManager::getInstance().playSound("explosion", 100.f);
        }
    
        if (proj.alive && terrain.checkCollision(proj.shape.getPosition())) {
            explosions.emplace_back(proj.shape.getPosition()); // 🔥 Lisää räjähdys
            proj.alive = false;
            std::vector<PixelInfo> destroyed = terrain.destroy(proj.shape.getPosition(), 50);
            spawnDebris(destroyed, proj.shape.getPosition());
            SoundManager::getInstance().playSound("explosion", 100.f);
        }
    }

    // 🔥 Poista kuolleet ammukset listasta
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile &p) {
            return !p.alive;
        }),
        projectiles.end()
    );

    // Tank 1 tuhoutuminen
    if (tank1.isDestroyed() && !tank1DestroyedFxDone) {
        explosions.emplace_back(tank1.getPosition());  // pieni tuliräjähdys
        tank1DestroyedFxDone = true;
    }
    // Tank 2 tuhoutuminen
    if (tank2.isDestroyed() && !tank2DestroyedFxDone) {
        explosions.emplace_back(tank2.getPosition());
        tank2DestroyedFxDone = true;
    }

    // Tankin tuhoaminen, jos se on ruudun alapuolella
    auto killIfOffscreen = [&](Tank& t, bool& fxDone)
    {
        const float FLOOR_Y = 1080.f;                 // ruudun ala-reuna
        if (!t.isDestroyed() && t.getPosition().y > FLOOR_Y)
        {
            t.takeDamage(t.getHp());                  // tyhjennä HP
            explosions.emplace_back(t.getPosition()); // visuaalinen räjähdys
            fxDone = true;                            // estä toistuva efekti
        }
    };
    killIfOffscreen(tank1, tank1DestroyedFxDone);
    killIfOffscreen(tank2, tank2DestroyedFxDone);

    // 🔥 Päivitä eventManager ja anna sille projektiililista
    eventManager.update(projectiles);

    if (waitingForTurnSwitch) { // 🔥 Odotetaan vuoron vaihtoa
        if (turnClock.getElapsedTime().asSeconds() >= 2.0f && !eventManager.anyProjectilesAlive(projectiles)) {
            eventManager.switchTurn(windForce, *this);  // 🔥 Lisätty `*this`
            waitingForTurnSwitch = false;
            eventManager.restartTurnTimer();
        }
    }


    // Päivitetään partikkelien sijainti ja tarkistetaan törmäykset maastoon
    for (auto &d : debrisList) {
        // 1) Liikutetaan partikkelia
        d.position += d.velocity * deltaTime;
        
        // 2) Painovoima
        d.velocity.y += gravity * 0.5f * deltaTime;
        
        // 3) Tarkistetaan törmääkö maahan
        if (d.position.x >= 0 && d.position.x < 1920 &&
            d.position.y >= 0 && d.position.y < 1080)
        {
            // Jos pikseli on "kiinteä" (maata) => pysäytä velocity
            if (terrain.checkCollision(d.position)) {
                // Yksinkertaisesti nostetaan partikkelia takaisin 1px,
                // jotta se ei uppoa maahan
                d.position.y -= 1.0f;
                // Pysäytetään velocity
                d.velocity = sf::Vector2f(0.f, 0.f);
            }
        }
        // Vähennetään elinaikaa
        d.lifetime -= deltaTime;
    }

        // Poista kuolleet partikkelit
        debrisList.erase(
            std::remove_if(debrisList.begin(), debrisList.end(),
                [](const Debris &d){ return d.lifetime <= 0; }),
            debrisList.end()
        );
}

/* =================================================
   DEBRIS & RESET GAME
   ================================================= */

void Game::spawnDebris(const std::vector<PixelInfo>& destroyedPixels, sf::Vector2f center) {
    // esim. rajoitetaan partikkelien maksimimäärää
    int maxParticles = 100;  
    int count = 0;

    for (auto &pixInfo : destroyedPixels) {
        if (count >= maxParticles) break;

        // Sijainti float-muodossa:
        sf::Vector2f pos((float)pixInfo.coords.x, (float)pixInfo.coords.y);

        // Haetaan pikselille satunnainen nopeus
        float vx = -100 + std::rand() % 201;   // 
        float vy = -100 + std::rand() % 201;  // 
        sf::Vector2f vel(vx, vy);

        // Väri 
        sf::Color col = pixInfo.color;  

        // Partikkelin elinaika
        float life = 9.0f + (std::rand() % 20) / 10.f;

        debrisList.emplace_back(pos, vel, col, life);
        count++;
    }
}


void Game::resetGame() {
    // Nollataan maasto ensin
    terrain.initialize(); // Luo uusi maasto ennen tankkien sijoittamista

    // Nollataan tankkien sijainnit ja tilat alkuperäisiin sijainteihin
    tank1.reset(terrain, tank1StartPosition); // Tarkistaa uuden maaston
    tank2.reset(terrain, tank2StartPosition); // Tarkistaa uuden maaston

    // Nollataan räjähdykset ja partikkelit
    explosions.clear(); // Tyhjennetään räjähdykset
    debrisList.clear(); // Tyhjennetään partikkelit

    // Nollataan tankkien tuhoamisflagit
    tank1DestroyedFxDone = false;
    tank2DestroyedFxDone = false;


    // Nollataan EventManager ja muut pelitilat
    eventManager.reset(tank1, tank2, *this);
    projectiles.clear(); // Tyhjennetään ammukset

    gravity = Config::GRAVITY;
    windForce = Config::getRandomWind();
    turnClock.restart();
    waitingForTurnSwitch = false;
}
