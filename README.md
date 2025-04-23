# 🚀 Tankkipeli – 2D‑tykistöpeli (C++17 + SFML 2.6)

Tämä projekti on **Scorched Tanks** ‑henkinen kahden pelaajan 2D‑peli, jossa tuhoat vastustajan tankin rikkomalla maastoa ja hallitsemalla tuulta.  Lähdekoodi on C++17:ää, grafiikka hoituu **SFML 2.6**:lla ja koko homma rakennetaan **CMake**lla.

> **Pika‑start**: Lataa uusin **Tankkipeli--win64.zip** → pura → tuplaklikkaa **Tankkipeli.exe** – ei asennusta!

---

## 🔑 Ominaisuudet

| ✔  | Ominaisuus                                               |
| -- | -------------------------------------------------------- |
| ✅  | Perlin‑noise‑pohjainen, hajotettava maasto (pikselitaso) |
| ✅  | Ballistinen fysiikka: painovoima, satunnainen tuuli      |
| ✅  | 30 s vuororajoitus, räjähdykset & partikkelit            |
| ✅  | Ääniefektit + taustamusiikki (SFML Audio)                |
| 🟡 | Useita aseita (WIP)                                      |
|    |                                                          |

---

## 📂 Kansiot ja tiedostot

```
├── assets/         # kuvat, fontit, äänet
├── docs/           # suomi‑ohjeet (käyttö, suunnittelu)
├── include/        # .hpp‑otsikot
├── src/            # .cpp‑lähdekoodi
├── build_release.bat  # Windowsin julkaisuskripti
├── CMakeLists.txt
└── vcpkg.json      # riippuvuuksien lukitus
```

### Lähdekoodit 

```
Config.hpp
EventManager.cpp / .hpp
Explosion.cpp / .hpp
GameCore.cpp  GameRender.cpp  Game.hpp
Projectile.cpp / .hpp
SoundManager.hpp
TankControl.cpp  TankGraphics.cpp  TankPhysics.cpp  Tank.hpp
Terrain.cpp / .hpp
UI.cpp / .hpp
main.cpp
menu.cpp / .hpp
version.cpp
```

---

## ⚙️ Riippuvuudet

- **CMake ≥ 3.15**
- **SFML 2.6** (suositus: *vcpkg* → `vcpkg install sfml`  tai manuaalisesti)  
- **C++17**‑kääntäjä (MSVC 19.3x, GCC 10+, Clang 12+)

---

## 🛠️ Kehittäjä‑build (Debug)

### Windows – Visual Studio 2022

```powershell
cmake -B build -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_CONFIGURATION_TYPES=Debug
cmake --build build --config Debug --parallel
build\Debug\Tankkipeli.exe
```

### Windows – MinGW‑w64

```bash
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
mingw32-make -C build -j8
build/Tankkipeli.exe
```

### Linux / macOS

```bash
sudo apt install build-essential cmake libsfml-dev
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
./build/Tankkipeli
```

---

## 📦 Release‑build (Kehittäjille & Testaajille)

### A. Windows‑64 bit – **build\_release.bat**

Skriptin ajaminen juurikansiosta:

```powershell
./build_release.bat
```

Tuotokseksi syntyy *dist/*‑hakemisto ja **Tankkipeli--win64.zip** jossa:

- `Tankkipeli.exe` (Release‑optimoitu, x64)
- `assets/` (kuvat, fontit, äänet)
- SFML‑DLL‑tiedostot + `version.txt`

> **Käyttäjälle**: pura zip → käynnistä `Tankkipeli.exe`.

### B. Manuaalinen Release (kaikki alustat)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build --config Release --parallel
```

- **Windows**: kopioi `build\Release\Tankkipeli.exe`, `assets/` ja SFML‑DLL‑kirjastot samaan kansioon.
- **Linux/macOS**: kopioi `assets/` samaan hakemistoon tai vie se `~/.local/share/tankkipeli/`.

---

## 🎮 Ohjaus

- **◄ / ►** – tykin kulma
- **▲ / ▼** – panoksen voima
- **A / D** – tankin liike
- **Hiiri** – tähtäys & laukaisu (LMB) / voima (rulla)
- **Esc** – Pause / valikko

---

## 🤝 Osallistujat

| Nimi       | Rooli        |
| ---------- | ------------ |
| *Täydennä* | Pääkehittäjä |

---

## 📜 Lisenssi

Koodi on julkaistu **MIT**‑lisenssillä – katso *LICENSE* jos lisäät koodia tai jaat peliä.

> *Palautteet, bugiraportit ja PR*\*:t\*\* ovat tervetulleita – kiitos kun kokeilit Tankkipeliä!\*

