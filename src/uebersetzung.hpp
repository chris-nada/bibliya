#pragma once

#include "buch.hpp"
#include <vector>
#include <functional>

class Uebersetzung final {

public:

    static void init(std::function<void(void)>& display_progress);

    static const std::unordered_map<std::string, std::unordered_map<std::string, Uebersetzung>>& get_uebersetzungen();

    const std::string& get_name() const { return name; }

    const std::string& get_info() const { return info; }

    const std::string& get_text(const std::string& osis_id) const;

private:

    /// Key: Sprache [de], Value: Uebersetzungen
    static std::unordered_map<std::string, std::unordered_map<std::string, Uebersetzung>> uebersetzungen;

    /// Name der Übersetzung
    std::string name;

    /// Kurzer Infotext der Übersetzung
    std::string info;

    /// Key: OSIS-ID [Job.42.15], Value: Text
    std::unordered_map<std::string, std::string> texte;

};
