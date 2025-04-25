#ifndef SOUNDMANAGER_HPP
#define SOUNDMANAGER_HPP

#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>

class SoundManager {
public:
    // Singleton
    static SoundManager& getInstance() {
        static SoundManager instance;
        return instance;
    }

    // Maksimi samanaikaisia ääni­kanavia
    static constexpr std::size_t MAX_CHANNELS = 32;

    // Bufferien ja musiikin lataus
    bool loadSound(const std::string& key, const std::string& filename) {
        sf::SoundBuffer buffer;
        if (!buffer.loadFromFile(filename)) return false;
        soundBuffers[key] = std::move(buffer);
        return true;
    }

    bool loadMusic(const std::string& key, const std::string& filename) {
        auto music = std::make_unique<sf::Music>();
        if (!music->openFromFile(filename)) return false;
        musics[key] = std::move(music);
        return true;
    }

    sf::Music* getMusic(const std::string& key) {
        auto it = musics.find(key);
        return (it == musics.end()) ? nullptr : it->second.get();
    }

    // Äänikanavien toisto
    void playSound(const std::string& key, float volume = 100.f) {
        auto it = soundBuffers.find(key);
        if (it == soundBuffers.end()) return;

        // Kierrätä pysähtyneet kanavat
        for (auto& up : sounds) {
            if (up->getStatus() == sf::Sound::Stopped) {
                up->setBuffer(it->second);
                up->setVolume(volume);
                up->play();
                return;
            }
        }

        // Jos ollaan kanavarajassa, pudotetaan vanhin kanava
        if (sounds.size() >= MAX_CHANNELS) {
            sounds.erase(sounds.begin());
        }

        // Luodaan uusi kanava heapille (oliot pysyvät paikallaan)
        auto s = std::make_unique<sf::Sound>();
        s->setBuffer(it->second);
        s->setVolume(volume);
        s->play();
        sounds.push_back(std::move(s));
    }

    // Päivitys peliloopissa: vapauta pysähtyneet kanavat
    void update() {
        for (int i = static_cast<int>(sounds.size()) - 1; i >= 0; --i) {
            if (sounds[i]->getStatus() == sf::Sound::Stopped) {
                sounds.erase(sounds.begin() + i);
            }
        }
    }

private:
    SoundManager() { sounds.reserve(MAX_CHANNELS); }
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    // Heap-varatut kanavat, oliot pysyvät paikallaan
    std::vector<std::unique_ptr<sf::Sound>> sounds;

    // SoundBuffers ja musiikit
    std::map<std::string, sf::SoundBuffer> soundBuffers;
    std::map<std::string, std::unique_ptr<sf::Music>> musics;
};

#endif // SOUNDMANAGER_HPP
