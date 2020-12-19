#pragma once

#include <vector>
#include <string>

class Lesezeichen final {

public:

    Lesezeichen() = default;

    Lesezeichen(const std::string& notiz, const std::string& buch, unsigned int kapitel, unsigned int vers);

    static std::vector<Lesezeichen>& alle();

    static void add(const Lesezeichen& neu);

    static void remove(std::size_t pos);

    std::string notiz;

    std::string buch;

    unsigned kapitel;

    unsigned vers;

    bool operator<(const Lesezeichen& rhs) const;

    template<class Archiv>
    void serialize(Archiv& ar) {
        ar(notiz, buch, kapitel, vers);
    }

private:

    static std::vector<Lesezeichen> lesezeichen;

    static void save();

};