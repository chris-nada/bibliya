#pragma once

#include <vector>
#include <string>

#if __has_include(<execution>)
    #include <execution>
    #define PAR_UNSEQ /* std::execution::par_unseq, TODO */
#else
    #define PAR_UNSEQ
#endif

class Sonstiges final {

public:

    enum class Format { NONE, GREY, RED, YELLOW, GREEN };

    static std::vector<std::string> tokenize(const std::string& text, const char token, unsigned max_tokens = 0) {
        std::vector<std::string> teile;
        teile.reserve(4);
        size_t anfang = 0, ende;
        while ((ende = text.find(token, anfang)) != std::string::npos && (max_tokens == 0 || teile.size() < max_tokens-1)) {
            teile.push_back(text.substr(anfang, ende - anfang));
            anfang = ende;
            anfang++;
        }
        teile.push_back(text.substr(anfang));
        return teile;
    }

    static std::string get_text_zwischen(std::string zeile, const std::string& s1 = ">", const std::string& s2 = "</") {
        auto it1 = zeile.find(s1);
        auto it2 = zeile.find(s2);
        if (it1 != std::string::npos) {
            ++it1;
            if (it2 != std::string::npos && it1 < it2) return {zeile.begin() + it1, zeile.begin() + it2};
            else return {zeile.begin() + it1, zeile.end()};
        }
        return {};
    }

    static void replace(std::string& s, const std::string& alt, const std::string& neu) {
        while (s.find(alt) != std::string::npos) s.replace(s.find(alt), alt.size(), neu);
    }

    static void tags_entfernen(std::string& s) {
        auto find = [&](std::size_t& i1, std::size_t& i2) {
            i1 = s.find('<');
            i2 = s.find('>');
        };
        size_t t1, t2;
        find(t1, t2);
        while (t1 != std::string::npos && t2 != std::string::npos && t1 < t2) {
            s.erase(s.begin()+t1, s.begin()+t2+1);
            find(t1, t2);
        }
    }

};
