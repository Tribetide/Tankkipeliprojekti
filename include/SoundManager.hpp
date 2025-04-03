#ifndef SOUNDMANAGER_HPP
#define SOUNDMANAGER_HPP

#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <stdexcept>

class SoundManager {
public:
    // Singleton: SoundManager::getInstance()
    static SoundManager& getInstance() {
        static SoundManager instance;
        return instance;
    }

    // 1. Lataa äänibuffer (lyhyt efekti)
    bool loadSound(const std::string& key, const std::string& filename) {
        sf::SoundBuffer buffer;
        if (!buffer.loadFromFile(filename)) {
            return false;
        }
        soundBuffers[key] = buffer;
        return true;
    }

    // 2. Soita lyhyt ääniefekti halutulla äänenvoimakkuudella (0..100)
    void playSound(const std::string& key, float volume = 100.f) {
        auto it = soundBuffers.find(key);
        if (it == soundBuffers.end()) {
            throw std::runtime_error("Sound buffer not found: " + key);
        }
        // Lisätään uusi sf::Sound vectoriin
        sounds.emplace_back();
        sf::Sound& sound = sounds.back();
        sound.setBuffer(it->second);
        sound.setVolume(volume);
        sound.play();
    }

    // 3. Lataa streamattava musiikki (esim. War.ogg)
    bool loadMusic(const std::string& key, const std::string& filename) {
        auto music = std::make_unique<sf::Music>();
        if (!music->openFromFile(filename)) {
            return false;
        }
        musics[key] = std::move(music);
        return true;
    }

    // 4. Hae sf::Music* (voidaan asettaa loop, volume, play, stop, tms.)
    sf::Music* getMusic(const std::string& key) {
        auto it = musics.find(key);
        if (it == musics.end()) {
            throw std::runtime_error("Music not found: " + key);
        }
        return it->second.get();
    }

    // 5. Kutsutaan pelisilmukan jokaisella kierroksella:
    //    SoundManager siivoaa pysähtyneet äänet pois (etteivät jää vektoriin).
    void update() {
        sounds.erase(
            std::remove_if(sounds.begin(), sounds.end(),
                           [](const sf::Sound& s){
                               return s.getStatus() == sf::Sound::Stopped;
                           }),
            sounds.end()
        );
    }

private:
    SoundManager() = default;
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    // Äänipuskurit lyhyille efekteille (ampuminen, räjähdys jne.)
    std::map<std::string, sf::SoundBuffer> soundBuffers;

    // Taustamusiikit (sf::Music ei ole kopioitava → säilytetään unique_ptr)
    std::map<std::string, std::unique_ptr<sf::Music>> musics;

    // Soivat sf::Sound-oliot. Välttämätön, jottei ääni katkea funktiosta poistuttaessa.
    std::vector<sf::Sound> sounds;
};

#endif // SOUNDMANAGER_HPP
