cmake_minimum_required(VERSION 3.15)
project(Tankkipeli)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 🔹 Lisätään include-kansio, jotta headerit löytyvät
include_directories(${CMAKE_SOURCE_DIR}/include)

# 🔹 Etsitään kaikki lähdetiedostot src-kansiosta
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# Kopioi fontit build-, Release-ja Debug-kansioihin
file(COPY assets/fonts DESTINATION ${CMAKE_BINARY_DIR}/assets)
file(COPY assets/fonts DESTINATION ${CMAKE_BINARY_DIR}/Debug/assets)
file(COPY assets/fonts DESTINATION ${CMAKE_BINARY_DIR}/Release/assets)

# 🔹Kopioi kuvat build-, Release-ja Debug-kansioihin
file(COPY assets/images DESTINATION ${CMAKE_BINARY_DIR}/assets)
file(COPY assets/images DESTINATION ${CMAKE_BINARY_DIR}/Debug/assets)
file(COPY assets/images DESTINATION ${CMAKE_BINARY_DIR}/Release/assets)

# Kopioi äänet build-, Release-ja Debug-kansioihin
file(COPY assets/sounds DESTINATION ${CMAKE_BINARY_DIR}/assets)
file(COPY assets/sounds DESTINATION ${CMAKE_BINARY_DIR}/Debug/assets)
file(COPY assets/sounds DESTINATION ${CMAKE_BINARY_DIR}/Release/assets)

# 🔹 Luodaan suoritettava ohjelma
add_executable(Tankkipeli ${SOURCES})

# 🔹 Linkitetään SFML automaattisesti
find_package(SFML 2.6 REQUIRED COMPONENTS graphics window system audio)
target_link_libraries(Tankkipeli PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

# ----- Jakeluun tarvittavat tiedostot -------------------------------

# 1. Kopioi exe + assets
install(TARGETS Tankkipeli RUNTIME DESTINATION .)
install(DIRECTORY assets DESTINATION .)

# 2. Kopioi SFML‑DLL:t _ilman_ kovakoodattua polkua
if (WIN32)
  foreach(_lib system window graphics audio)
    get_target_property(_dll sfml-${_lib} IMPORTED_LOCATION_RELEASE)
    list(APPEND SFML_DLLS "${_dll}")
  endforeach()

  # openal32.dll on samassa kansiossa kuin sfml-audio-2.dll
  get_filename_component(_AUDIO_DIR "${SFML_DLLS}" PATH)
  set(OPENAL_DLL "${_AUDIO_DIR}/openal32.dll")

  install(FILES ${SFML_DLLS} ${OPENAL_DLL} DESTINATION .)
endif()

# 3. CPack – ZIP pakkauksen ulostulokansio = projektin juuri
set(CPACK_PACKAGE_NAME        "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION     "${PROJECT_VERSION}")
set(CPACK_GENERATOR           "ZIP")

# ⬇️ tämä rivi ohjaa ZIP‑tiedoston juuri‑kansioon, ei build/:iin
set(CPACK_OUTPUT_FILE_PREFIX  "${CMAKE_SOURCE_DIR}")

include(CPack)






