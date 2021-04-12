#pragma once

#include "buch.hpp"

/// Enthält Textstelle, die ein Tab anzeigen soll
class Tab {

public:

    /// Default Ctor.
    Tab() = default;

    /// (Anzuzeigendes Buch (initialisiert).
    const mutable Buch* buch = nullptr;

    /// Anzuzeigendes Buch.
    unsigned buch_pos = 1;

    /// Anzuzeigendes Kapitel.
    unsigned auswahl_kapitel = 1;

    /// (Erster) anzuzeigender Vers.
    unsigned auswahl_vers = 1;

    /// 0 = Einzeln, 1 = Fünf Verse, 2 = Kapitel.
    int auswahl_modus = 2;

    /// Getter: Buch.
    const Buch* get_buch() const {
        if (buch == nullptr) buch = &Buch::get_buch(1);
        return buch;
    }

    /// Liefert einen Namen für die aktuelle Textstelle: Mat 1:2
    std::string get_name() const {
        return get_buch()->get_key() + " " + std::to_string(auswahl_kapitel) + ':' + std::to_string(auswahl_vers);
    }

    /// Serialisierungsmethode für Cereal.
    template<class Archiv>
    void serialize(Archiv& ar) {
        ar(buch_pos, auswahl_kapitel, auswahl_vers, auswahl_modus);
    }

};
