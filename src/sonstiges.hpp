#pragma once

#include <vector>
#include <string>

class Sonstiges final {

public:

    enum class Format { NONE, GREY, RED, YELLOW, GREEN };

    static std::vector<std::string> tokenize(const std::string& text, const char token) {
        std::vector<std::string> teile;
        size_t anfang = 0, ende;
        while ((ende = text.find(token, anfang)) != std::string::npos) {
            teile.push_back(text.substr(anfang, ende - anfang));
            anfang = ende;
            anfang++;
        }
        teile.push_back(text.substr(anfang));
        return teile;
    }

};
