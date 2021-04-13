#pragma once

#include "buch.hpp"
#include "lesezeichen.hpp"

#include <vector>
#include <functional>
#include <filesystem>
#include <tbb/mutex.h>

/**
 * Klasse enthält den kompletten Text *einer* Übersetzung (in der Variable `texte`).
 */
class Uebersetzung final {

    static inline tbb::mutex uebersetzungs_mutex;
    static inline tbb::mutex anim_mutex;

public:

    Uebersetzung();

    static void init(std::function<void(void)>& display_progress);

    static const std::unordered_map<std::string, std::unordered_map<std::string, Uebersetzung>>& get_uebersetzungen();

    static std::vector<Lesezeichen> suche(const std::string& suchbegriff, bool nur_nt);

    const std::string& get_text(const std::string& osis_id) const;

    const std::string& get_name() const { return name; }

    const std::string& get_info() const { return info; }

private:

    static void import_osis(const std::string& sprache, const std::filesystem::directory_entry& datei,
                            std::function<void(void)>& display_progress);

    static void import_csv(const std::string& sprache, const std::filesystem::directory_entry& datei,
                           std::function<void(void)>& display_progress);

    /// Key: Sprache [de], Value: Uebersetzungen
    static std::unordered_map<std::string, std::unordered_map<std::string, Uebersetzung>> uebersetzungen;

private:

    /// Name der Übersetzung
    std::string name;

    /// Kurzer Infotext der Übersetzung
    std::string info;

    /// Key: OSIS-ID [Job.42.15], Value: Text
    std::unordered_map<std::string, std::string> texte;

};
