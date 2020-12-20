#pragma once

#include <string>
#include <unordered_map>

class Buch final {

    friend class Uebersetzung;

public:

    /// [ID] [Buch]
    static const std::unordered_map<std::string, Buch>& get_buecher() { return buecher; }

    /// Liefert Buch nach Position.
    static const Buch& get_buch(unsigned pos);

    /// Liefert Buch nach ID.
    static const Buch& get_buch(const std::string& key) { return buecher.at(key); }

    /// Plaintext Name des Buches
    const std::string& get_name() const { return name; }

    /// Getter: [ID]
    const std::string& get_key() const { return key; }

    /// Getter: Position.
    unsigned get_pos() const { return pos; };

    /// OSIS-ID für gg. Kapitel + Vers
    const std::string& get_osis_id(unsigned kapitel, unsigned vers) const;

    /// Anzahl der Kapitel in diesem Buch
    unsigned get_n_kapitel() const { return n_kapitel; }

    /// Anzahl der Verse in diesem Buch
    unsigned get_n_verse(unsigned kapitel) const;

private:

    /// <Position,Name>, (id)
    static const std::tuple<unsigned, std::string>& get_order(const std::string& id);

    static void buch_osis_check(const std::string& osis_id, unsigned kapitel, unsigned vers);

    /// Key: Kapitel [key], Value: Kapitel
    static std::unordered_map<std::string, Buch> buecher;

    /// Name Plaintext
    std::string name;

    /// Key (in map).
    std::string key;

    /// Position
    unsigned pos;

    /// [Kapitel][Vers] -> OSIS-ID
    std::unordered_map<unsigned, std::unordered_map<unsigned, std::string>> osis_ids;

    /// Anzahl Kapitel
    unsigned n_kapitel = 0;

    /// Anzahl Verse für Kapitel
    std::unordered_map<unsigned, unsigned> n_verse;

};
