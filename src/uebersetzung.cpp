#include "uebersetzung.hpp"
#include "sonstiges.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <mutex>

#if __has_include(<execution>)
    #include <execution>
    #define PAR_UNSEQ std::execution::par_unseq,
#else
    #define PAR_UNSEQ
#endif

std::unordered_map<std::string, std::unordered_map<std::string, Uebersetzung>> Uebersetzung::uebersetzungen;

Uebersetzung::Uebersetzung() {
    texte.reserve(Buch::get_buecher().size());
}

const std::string& Uebersetzung::get_text(const std::string& osis_id) const {
    try { return texte.at(osis_id); }
    catch (const std::exception& e) {
        static std::string nichts = "-";
        return nichts;
    }
}

const std::unordered_map<std::string, std::unordered_map<std::string, Uebersetzung>>& Uebersetzung::get_uebersetzungen() {
    return uebersetzungen;
}

std::vector<Lesezeichen> Uebersetzung::suche(const std::string& suchbegriff, bool nur_nt) {
    static std::mutex mutex;
    std::vector<Lesezeichen> ergebnisse;
    std::for_each(PAR_UNSEQ Buch::get_buecher().begin(), Buch::get_buecher().end(), [&](const auto& buch_paar) {
        const Buch& buch = buch_paar.second;
        if (!nur_nt ||
            (buch.get_pos() >= Buch::get_buch("Matt").get_pos() &&
             buch.get_pos() <= Buch::get_buch("Rev").get_pos()))
        {
            for (unsigned kapitel = 1; kapitel <= buch.get_n_kapitel(); ++kapitel) {
                for (unsigned vers = 1; vers <= buch.get_n_verse(kapitel); ++vers) {
                    const std::string& osis_id = buch.get_osis_id(kapitel, vers);
                    for (const auto& sprach_paar : uebersetzungen) {
                        for (const auto& u_paar : sprach_paar.second) {
                            const Uebersetzung& u = u_paar.second;
                            const std::string& text = u.get_text(osis_id);

                            // Begriff gefunden?
                            if (text.find(suchbegriff) != std::string::npos) {
                                std::lock_guard lock(mutex);
                                ergebnisse.emplace_back("", buch.get_key(), kapitel, vers);
                                goto naechster_vers;
                            }
                        }
                    }
                    naechster_vers:;
                }
            }
        }
    });
    std::sort(ergebnisse.begin(), ergebnisse.end());
    return ergebnisse;
}

void Uebersetzung::init(std::function<void(void)>& display_progress) {
    auto timer = std::chrono::system_clock::now();
    if (uebersetzungen.empty()) {
        // [Sprache] [Pfad]
        std::vector<std::pair<std::string, std::filesystem::directory_entry>> dateien;
        for (const auto& sprachordner : std::filesystem::directory_iterator("data/buecher")) {
            if (sprachordner.is_directory()) {
                const std::string& sprache = sprachordner.path().filename().string();
                for (const auto& datei : std::filesystem::directory_iterator(sprachordner)) {
                    if (datei.is_regular_file() && (
                            datei.path().string().find(".xml") != std::string::npos ||
                            datei.path().string().find(".csv") != std::string::npos)) {
                        dateien.emplace_back(sprache, datei);
                    }
                }
            }
        }

        // Dateien einlesen
        static std::mutex u_mutex; // Hinzufügen neuer Übersetzungen
        static std::mutex anim_mutex;
        std::for_each(PAR_UNSEQ dateien.begin(), dateien.end(), [&](const auto& paar) {
        //for (const auto& paar : dateien) {
            const std::string& sprache = paar.first;
            const std::filesystem::directory_entry& datei = paar.second;
            if (datei.path().string().find(".xml") != std::string::npos) {
                import_osis(sprache, datei, display_progress);
            } else if (datei.path().string().find(".csv") != std::string::npos) {
                import_csv(sprache, datei, display_progress);
            }
            else std::cerr << "Ungueltiges Dateiformat: " << datei.path().string() << std::endl;
        });
        const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - timer
        );
        std::cout << "Ladebenchmark: " << (millis.count() / 1000.f) << 's' << std::endl;
    }
}

void Uebersetzung::import_osis(
        const std::string& sprache, const std::filesystem::directory_entry& datei,
        std::function<void(void)>& display_progress)
{
    if (std::ifstream in(datei.path()); in.good()) {

        /// Text importieren - OSIS-ID, Text
        std::cout << "Uebersetzung wird importiert, Sprache: " << sprache << ' ' << datei.path().string() << std::endl;

        Uebersetzung u;
        std::string trennchar;

        for (std::string s; std::getline(in, s);) {

            // Ladeanimation
            if (thread_local unsigned counter = 0; counter++ % 5000 == 0) {
                std::lock_guard lock(anim_mutex);
                display_progress();
            }

            // Vers Start
            if (s.find("verse") != std::string::npos && s.find("osisID") != std::string::npos) {
                if (trennchar.empty()) {
                    const auto pos1 = s.find('\'');
                    const auto pos2 = s.find('\"');
                    trennchar = pos1 < pos2 ? "'" : "\"";
                }
                std::string osis_id, txt;
                try {
                    osis_id = {s.begin() + s.find("osisID"), s.begin() + s.find(trennchar) + 13};
                    osis_id = {osis_id.begin() + osis_id.find(trennchar) + 1,
                               osis_id.begin() + osis_id.find_last_of(trennchar)};
                    txt = Sonstiges::get_text_zwischen(s, ">", "<");
                } catch (const std::exception& e) {
                    std::cerr << "[Warnung] Fehler beim Vers-Parsen: " << e.what() << ' ' << s << std::endl;
                    continue;
                }
                //std::cout << "\tOSIS-ID: " << osis_id << '\n';
                //std::cout << "\t\ttxt: " << txt << std::endl;

                // Fehlerbehandlung
                if (osis_id.empty()) {
                    std::cerr << "[Warnung] osis_id leer: " << datei << ' ' << s << '\n';
                    continue;
                }

                // Weitere Zeile auslesen
                if (txt.empty()) {
                    /*
                     * In solchen Fällen:
                     *
                     * <verse osisID='2Chr.1.13'>
                     *     <note>Anmerkung...</note>Text.</verse>
                     *
                     * oder
                     *
                     * <verse osisID="Ps.118.9" sID="Ps.118.9.seID.16674" n="9" />
                     * <hi type="italic">Beth</hi> In quo corrigit adolescentior viam suam? in custodiendo sermones tuos.
                     */
                    std::string s2;
                    std::getline(in, s2);
                    Sonstiges::replace(s2, "<note>", "[");
                    Sonstiges::replace(s2, "</note>", "]");
                    for (unsigned limit = 0; limit < 5 &&
                                             s2.find('<') != std::string::npos &&
                                             s2.find('>') != std::string::npos; ++limit) {
                        s2.erase(s2.find('<'), s2.find('>'));
                    }
                    // Leading Whitespace entfernen
                    const auto first_char = s2.find_first_not_of(" \t");
                    if (first_char > 0 && first_char != std::string::npos) s2 = s2.substr(first_char);
                    txt.append(s2);
                    if (txt.empty()) std::cerr << "[Warnung] txt leer: s=" << s << ", s2=" << s2 << '\n';
                }

                // Anmerkungen
                Sonstiges::replace(txt, "<note>", "[");
                Sonstiges::replace(txt, "</note>", "]");
                Sonstiges::tags_entfernen(txt);

                // Buch/Kapiteleintrag schreiben
                const auto& tokens = Sonstiges::tokenize(osis_id, '.');
                if (tokens.size() == 3) {
                    try {
                        const unsigned kapitel = std::stoul(tokens[1]);
                        const unsigned vers = std::stoul(tokens[2]);
                        Buch::buch_osis_check(tokens[0], kapitel, vers);
                    } catch (const std::exception& e) {
                        std::cerr << "[Warnung] osis_id Parse Fehler: " << osis_id << '\n';
                    }
                }
                else std::cerr << "[Warnung] osis_id tokens != 3 in " << datei << ' ' << osis_id << '\n';

                // Speichern
                u.texte[osis_id].append(txt);
            }
                // Titel
            else if (u.name.empty() && s.find("title") != std::string::npos) {
                u.name = Sonstiges::get_text_zwischen(s);
            }
                // Beschreibung
            else if (u.info.empty() && s.find("description") != std::string::npos) {
                u.info = Sonstiges::get_text_zwischen(s);
            }
        }
        // Speichern
        const std::string key = datei.path().filename().string();
        std::cout << "\tImport von " << key << " abgeschlossen" << std::endl;
        std::lock_guard lock(uebersetzungs_mutex);
        uebersetzungen[sprache][key] = u;
    }
    else std::cerr << "[Warnung] Uebersetzung konnte nicht geoeffnet werden: " << datei.path().string() << std::endl;
}

void Uebersetzung::import_csv(const std::string& sprache, const std::filesystem::directory_entry& datei,
                              std::function<void(void)>& display_progress) {
    // Kapitelzuordnung einlesen
    static auto build_map = []() {
        std::unordered_map<std::string, std::string> map;
        if (std::ifstream in("data/mbz_kapitel.ini"); in.good()) {
            for (std::string zeile; std::getline(in, zeile);) {
                if (zeile.size() <= 3 || zeile.find(',') == std::string::npos) continue;
                const auto& tokens = Sonstiges::tokenize(zeile, ',');
                if (tokens.size() != 2 || tokens[0].empty() || tokens[1].empty()) continue;
                map[tokens[0]] = tokens[1];
            }
        }
        return map;
    };
    static const std::unordered_map<std::string, std::string> osis_buch_map = build_map();

    if (std::ifstream in(datei.path()); in.good()) {
        /// Text importieren - CSV-ID, Text
        std::cout << "Uebersetzung wird importiert, Sprache: " << sprache << ' ' << datei.path().string() << std::endl;

        // Titel, Info = 1. & 2. Zeile
        Uebersetzung u;
        std::string zeile;
        std::getline(in, u.name);
        std::getline(in, u.info);

        // Zeilen (=Verse) auslesen
        for (; std::getline(in, zeile); ) {
            // Ladeanimation
            if (thread_local unsigned counter = 0; counter++ % 5000 == 0) {
                std::lock_guard lock(anim_mutex);
                display_progress();
            }
            if (zeile.size() < 8) continue;
            if (zeile.find(',') == std::string::npos) continue;
            auto tokens = Sonstiges::tokenize(zeile, ',', 4);
            if (tokens.size() != 4) {
                std::cerr << "\t[Warnung] Ungueltige Zeile (" << tokens.size() << ") in: " << datei.path().string() << ' ' << zeile << std::endl;
                continue;
            }

            // OSIS-ID erzeugen
            try {
                if (osis_buch_map.count(tokens[0])) tokens[0] = osis_buch_map.at(tokens[0]);
                const unsigned kapitel = std::stoul(tokens[1]);
                const unsigned vers = std::stoul(tokens[2]);
                Buch::buch_osis_check(tokens[0], kapitel, vers);
                const std::string osis_id = tokens[0] + "." + tokens[1] + "." + tokens[2];

                std::string& verstext = tokens[3];
                Sonstiges::tags_entfernen(verstext);
                u.texte[osis_id] = verstext;
            } catch (const std::exception& e) {
                std::cerr << "[Warnung] osis_id Parse Fehler: " << zeile << '\n';
            }
        }
        // Speichern
        const std::string key = datei.path().filename().string();
        std::cout << "\tImport von " << key << " abgeschlossen" << std::endl;
        std::lock_guard lock(uebersetzungs_mutex);
        uebersetzungen[sprache][key] = u;
    }
}


