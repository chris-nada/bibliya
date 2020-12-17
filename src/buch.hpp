#pragma once

#include <string>
#include <unordered_map>

class Buch final {

    friend class Uebersetzung;

public:

    /// Plaintext Name des Buches
    const std::string& get_name() const { return name; }

    /// OSIS-ID für gg. Kapitel + Vers
    const std::string& get_osis_id(unsigned kapitel, unsigned vers) const {
        try { return osis_ids.at(kapitel).at(vers); }
        catch (const std::exception& e) {
            static const std::string nichts = "-";
            return nichts;
        }
    }

    /// Anzahl der Kapitel in diesem Buch
    unsigned get_n_kapitel() const { return n_kapitel; }

    /// Anzahl der Verse in diesem Buch
    unsigned get_n_verse(unsigned kapitel) const {
        try { return n_verse.at(kapitel); }
        catch (const std::exception& e) { return 0; }
    }

private:

    /// Name Plaintext
    std::string name;

    /// [Kapitel][Vers] -> OSIS-ID
    std::unordered_map<unsigned, std::unordered_map<unsigned, std::string>> osis_ids;

    /// Anzahl Kapitel
    unsigned n_kapitel;

    /// Anzahl Verse für Kapitel
    std::unordered_map<unsigned, unsigned> n_verse;

};
