cmake_minimum_required(VERSION 3.15)
project(Tankkipeli)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 🔹 Lisätään include-kansio, jotta headerit löytyvät
include_directories(${CMAKE_SOURCE_DIR}/include)

# 🔹 Etsitään kaikki lähdetiedostot src-kansiosta
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# Kopioi fontit build-kansioon
file(COPY assets/fonts DESTINATION ${CMAKE_BINARY_DIR}/assets)

# Kopioi fontit myös Debug-kansioon
file(COPY assets/fonts DESTINATION ${CMAKE_BINARY_DIR}/Debug/assets)

# 🔹 Kopioi kuvat build- ja Debug-kansioihin
file(COPY assets/images DESTINATION ${CMAKE_BINARY_DIR}/assets)
file(COPY assets/images DESTINATION ${CMAKE_BINARY_DIR}/Debug/assets)

# Kopioi äänet build- ja Debug-kansioihin
file(COPY assets/sounds DESTINATION ${CMAKE_BINARY_DIR}/assets)
file(COPY assets/sounds DESTINATION ${CMAKE_BINARY_DIR}/Debug/assets)

# 🔹 Luodaan suoritettava ohjelma
add_executable(Tankkipeli ${SOURCES})

# 🔹 Linkitetään SFML automaattisesti
find_package(SFML 2.6 REQUIRED COMPONENTS graphics window system audio)
target_link_libraries(Tankkipeli PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

set(SOURCES
    ...
    src/Explosion.cpp  # lisää tämä rivi jos puuttuu
)





