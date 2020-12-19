#pragma once

#include <vector>
#include <string>

class Sonstiges final {

public:

    enum class Format { NONE, GREY, RED, YELLOW, GREEN };

    static std::vector<std::string> tokenize(const std::string& text, const char token) {
        std::vector<std::string> teile;
        teile.reserve(3);
        size_t anfang = 0, ende;
        while ((ende = text.find(token, anfang)) != std::string::npos) {
            teile.push_back(text.substr(anfang, ende - anfang));
            anfang = ende;
            anfang++;
        }
        teile.push_back(text.substr(anfang));
        return teile;
    }

    static std::string get_text_zwischen(std::string zeile, const std::string& s1 = ">", const std::string& s2 = "</") {
        const auto tag_ende = zeile.find(s1);
        if (tag_ende != std::string::npos) {
            zeile = zeile.substr(tag_ende + 1);
            const auto tag_begin = zeile.find(s2);
            if (tag_begin != std::string::npos) zeile.resize(tag_begin);
            return zeile;
        }
        return "";
    }

    static void replace(std::string& s, const std::string& alt, const std::string& neu) {
        while (s.find(alt) != std::string::npos) s.replace(s.find(alt), alt.size(), neu);
    }

};
