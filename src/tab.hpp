#pragma once

#include <iostream>
#include "buch.hpp"

/// Enthält Textstelle, die ein Tab anzeigen soll
class Tab {

public:

    /// Default Ctor.
    Tab() = default;

    /// Setter: Buch
    void set_buch(const Buch* buch);

    /// Getter: Buch.
    const Buch* get_buch() const;

    /// Liefert einen Namen für die aktuelle Textstelle: Mat 1:2
    std::string get_name() const;

    /// Serialisierungsmethode für Cereal.
    template<class Archiv>
    void serialize(Archiv& ar) {
        ar(buch_pos, auswahl_kapitel, auswahl_vers, auswahl_modus);
    }

    /// Anzuzeigendes Kapitel.
    unsigned auswahl_kapitel = 1;

    /// (Erster) anzuzeigender Vers.
    unsigned auswahl_vers = 1;

    /// 0 = Einzeln, 1 = Fünf Verse, 2 = Kapitel.
    int auswahl_modus = 2;

private:

    /// (Anzuzeigendes Buch (initialisiert).
    const mutable Buch* buch = nullptr;

    /// Anzuzeigendes Buch.
    unsigned buch_pos = 1;

};
