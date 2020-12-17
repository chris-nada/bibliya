#pragma once

#include "buch.hpp"
#include <vector>

class Uebersetzung final {

public:

    static const std::unordered_map<std::string, std::unordered_map<std::string, Uebersetzung>>& get_uebersetzungen();

    static const std::unordered_map<std::string, Buch>& get_buecher();

    const std::string& get_name() const { return name; }

    const std::string& get_text(const std::string& osis_id) const;

private:

    /// Key: Sprache [de], Value: Uebersetzungen
    static std::unordered_map<std::string, std::unordered_map<std::string, Uebersetzung>> uebersetzungen;

    /// Key: Kapitel [key], Value: Kapitel
    static inline std::unordered_map<std::string, Buch> buecher;

    /// Name der Übersetzung
    std::string name;

    /// Kurzer Infotext der Übersetzung
    std::string info;

    /// Key: OSIS-ID [Job.42.15], Value: Text
    std::unordered_map<std::string, std::string> texte;

};
