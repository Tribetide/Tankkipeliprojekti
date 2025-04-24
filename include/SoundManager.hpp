#ifndef SOUNDMANAGER_HPP
#define SOUNDMANAGER_HPP

#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <memory>
#include <vector>


class SoundManager {
public:
    // Singleton
    static SoundManager& getInstance() {
        static SoundManager instance;      
        return instance;
    }

    // Bufferien ja musiikin lataus
    bool loadSound(const std::string& key, const std::string& filename)
    {
        sf::SoundBuffer buffer;
        if (!buffer.loadFromFile(filename)) return false;
        soundBuffers[key] = buffer;
        return true;
    }

    bool loadMusic(const std::string& key, const std::string& filename)
    {
        auto music = std::make_unique<sf::Music>();
        if (!music->openFromFile(filename)) return false;
        musics[key] = std::move(music);
        return true;
    }

    sf::Music* getMusic(const std::string& key)
    {
        auto it = musics.find(key);
        return (it == musics.end()) ? nullptr : it->second.get();
    }

    //Äänikanavien toisto
    void playSound(const std::string& key, float volume = 100.f)
    {
        auto it = soundBuffers.find(key);
        if (it == soundBuffers.end()) return;           

        // Käytetään kierrätettäviä kanavia
        for (auto& s : sounds) {
            if (s.getStatus() == sf::Sound::Stopped) {
                s.setBuffer(it->second);
                s.setVolume(volume);
                s.play();
                return;
            }
        }

        // Luodaan uusi kanava, jos kaikki ovat varattuja
        sounds.emplace_back();                         
        sf::Sound& s = sounds.back();
        s.setBuffer(it->second);
        s.setVolume(volume);
        s.play();
    }

    // Päivitys peliloopissa
    void update()
    {
        sounds.erase(
            std::remove_if(sounds.begin(), sounds.end(),
                           [](const sf::Sound& s){
                               return s.getStatus() == sf::Sound::Stopped;
                           }),
            sounds.end());
    }

private:
    // Singletonin yksityinen konstruktori
    SoundManager() { sounds.reserve(64); }              // varataan kanavat
    SoundManager(const SoundManager&)            = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    // Kanavat ja äänitiedostot
    std::vector<sf::Sound> sounds;                      // kierrätettävät kanavat
    std::map<std::string, sf::SoundBuffer> soundBuffers;
    std::map<std::string, std::unique_ptr<sf::Music>> musics;
};

#endif // SOUNDMANAGER_HPP
