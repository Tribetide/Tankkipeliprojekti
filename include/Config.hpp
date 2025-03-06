#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

namespace Config {
    const std::string FONT_PATH_1 = "assets/fonts/arial.ttf";       // 🔥 Oletuspolku
    const std::string FONT_PATH_2 = "../assets/fonts/arial.ttf";    // 🔥 Jos ajetaan `build/`
    const std::string FONT_PATH_3 = "../../assets/fonts/arial.ttf"; // 🔥 Jos ajetaan `build/Debug/`
}

#endif
