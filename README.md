# 🚀 Tankkipeli

Tämä on **Scorched Tanks** -tyylinen 2D-tankkipeli, joka on toteutettu **C++:lla, SFML:llä ja CMake:lla**.  
Peli sisältää **rikkoutuvan maaston, ammuksen fysiikan (painovoima & tuuli) sekä vuoropohjaisen pelimekaniikan**.

🎯 **Tavoitteena** on ampua vastustajan tankki pois kentältä, samalla halliten tuulen ja ammuksen lentorataa!

---

## 🔹 Riippuvuudet

Ennen kuin voit kääntää ja ajaa pelin, varmista että sinulla on seuraavat asennettuna:

- **C++-kääntäjä** (MSVC, GCC tai Clang)
- **CMake** (vähintään 3.15)
- **SFML 2.6** (asennetaan joko vcpkg:llä tai käsin)

Jos käytät **vcpkg:tä**, voit asentaa SFML:n näin:
```sh
vcpkg install sfml
```
Jos **et käytä vcpkg:tä**, lataa SFML osoitteesta:  
📝 [https://www.sfml-dev.org/download.php](https://www.sfml-dev.org/download.php)

---

## 🔹 Asennus ja kääntäminen

Tämä projekti tukee **sekä MinGW (GCC) että MSVC (Visual Studio)** -ympäristöjä.

### **🟢 Windows (Visual Studio 2022)**
1️⃣ Avaa **PowerShell / Git Bash** ja siirry projektin kansioon:
```sh
cd C:\Users\HP\OneDrive\Asiakirjat\GitHub\Tankkipeli
```
2️⃣ Poista vanhat build-tiedostot:
```sh
rm -rf build
```
3️⃣ Luo uusi build-kansio ja konfiguroi CMake:
```sh
cmake -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake ..
```
4️⃣ Rakenna projekti:
```sh
cmake --build .
```
5️⃣ Aja peli:
```sh
./Tankkipeli.exe
```

---

### **🟢 Windows (MinGW / GCC)**
1️⃣ Poista vanha build-kansio:
```sh
rm -rf build
```
2️⃣ Konfiguroi CMake käyttämään MinGW:tä:
```sh
cmake -G "MinGW Makefiles" ..
```
3️⃣ Rakenna peli:
```sh
mingw32-make
```
4️⃣ Aja peli:
```sh
./Tankkipeli.exe
```

---

### **🟢 Linux/macOS (GCC/Clang)**
1️⃣ Asenna tarvittavat paketit:
```sh
sudo apt install build-essential cmake libsfml-dev
```
2️⃣ Luo build-kansio ja konfiguroi CMake:
```sh
mkdir build && cd build
cmake ..
make
```
3️⃣ Aja peli:
```sh
./Tankkipeli
```

---

## 🎮 Pelin ohjaus (Controls)
- **Nuolinäppäimet**: Liikuta tankkia
- **A ja D**: Säädä tykin kulmaa
- **W ja S**: Säädä laukaisuvoimaa
- **Välilyönti (Space)**: Ammu
- **Esc**: Poistu pelistä

---

## 📂 Projektin rakenne
```
📆 Tankkipeli
 ├🗂 src/           # Lähdekooditiedostot
 │ ├📝 main.cpp     # Pääohjelma
 │ ├📝 Game.cpp     # Pelin logiikka
 │ ├📝 Tank.cpp     # Tankin hallinta
 │ ├📝 Projectile.cpp # Ammuksen hallinta
 │ ├📝 Terrain.cpp  # Maaston käsittely
 ├🗂 include/       # Otsikkotiedostot
 │ ├📝 Game.hpp     # Pelilogiikan määrittelyt
 │ ├📝 Tank.hpp     # Tankkiin liittyvät määrittelyt
 │ ├📝 Projectile.hpp # Ammuksen määrittelyt
 │ ├📝 Terrain.hpp  # Maaston määrittelyt
 ├🗂 assets/        # Grafiikka- ja äänitiedostot
 ├🗂 tests/         # Testitiedostot
 ├📝 CMakeLists.txt # CMake-konfiguraatio
 ├📝 .gitignore     # GitHubin ignorointitiedosto
 ├📝 README.md      # Tämä tiedosto
 └📝 LICENSE        # Lisenssitiedosto (valinnainen)
```

---

## 🔮 Tulevat ominaisuudet (Backlog)
✅ Rikkoutuva maasto  
✅ Tankkien liikkuminen  
✅ Ammuksen fysiikka (tuuli & painovoima)  
🟡 Eri aseet (räjähtävä ja sirpaleammus)  
🟡 Pelaajien vuoromekaniikka  
🟡 Ääniefektit ja musiikki  
🔴 Online-moninpeli  

---

## 👥 Tekijät


## 📝 Lisenssi


