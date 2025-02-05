cmake_minimum_required(VERSION 3.15)
project(SFML_Tankkipeli)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 🔹 Tarkistetaan, käytetäänkö MinGW:tä
if(MINGW)
    message(STATUS "Käytetään MinGW:tä")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
endif()

# 🔹 Tarkistetaan, käytetäänkö vcpkg:tä
if(DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Käytetään vcpkg:tä SFML:n löytämiseen.")
else()
    message(STATUS "Etsitään SFML manuaalisesti ilman vcpkg:tä...")
endif()

# 🔹 Etsitään SFML ilman kovakoodattuja polkuja
find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

# 🔹 Lisää hakemistot (jos käytössä)
include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# 🔹 Luodaan ohjelma
add_executable(Tankkipeli ${SOURCES})

# 🔹 Linkitetään SFML automaattisesti
target_link_libraries(Tankkipeli PRIVATE sfml-graphics sfml-window sfml-system)
