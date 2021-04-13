#pragma once

#include "tab.hpp"

/// Repräsentiert ein einzelnes Lesezeichen, d.h. eine Referenz zu einer Textstelle.
class Lesezeichen final {

public:

    Lesezeichen() = default;

    Lesezeichen(const std::string& notiz, const std::string& buch, unsigned int kapitel, unsigned int vers);

    static void show(const char* id_lesezeichen, bool* open_lesezeichen, Tab& aktueller_tab);

    static std::vector<Lesezeichen>& alle();

    static void add(const Lesezeichen& neu);

    static void remove(std::size_t pos);

    /// Optionale Notiz des Nutzers.
    std::string notiz;

    /// Lesezeichen: Key des Buches.
    std::string buch;

    /// Lesezeichen: Kapitel.
    unsigned kapitel;

    /// Lesezeichen: Vers.
    unsigned vers;

    bool operator<(const Lesezeichen& rhs) const;

    template<class Archiv>
    void serialize(Archiv& ar) {
        ar(notiz, buch, kapitel, vers);
    }

private:

    /// Speicher für alle Lesezeichen.
    static std::vector<Lesezeichen> lesezeichen;

    /// Speichert alle aktuellen Lesezeichen.
    static void save();

};
