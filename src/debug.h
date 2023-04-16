#include <string>

namespace debug {

enum Color { black = 30, red = 31, green = 32, yellow = 33, blue = 34, purple = 35, darkgreen = 36, white = 37 };

inline std::string change_color(const std::string &str, Color color) {
    switch (color) {
        case black :
            return "\033[0;30m" + str + "\033[0m";
        case red :
            return "\033[0;31m" + str + "\033[0m";
        case green:
            return "\033[0;32m" + str + "\033[0m";
        case yellow :
            return "\033[0;33m" + str + "\033[0m";
        case blue:
            return "\033[0;34m" + str + "\033[0m";
        case purple:
            return "\033[0;35m" + str + "\033[0m";
        case darkgreen:
            return "\033[0;36m" + str + "\033[0m";
        case white:
            return "\033[0;37m" + str + "\033[0m";
        default:
            return str;
    }
}

} // namespace debug
