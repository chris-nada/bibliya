#include "uebersetzung.hpp"
#include "sonstiges.hpp"
#include <pugixml.hpp>
#include <filesystem>
#include <iostream>
#include <fstream>

std::unordered_map<std::string, std::unordered_map<std::string, Uebersetzung>> Uebersetzung::uebersetzungen;

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

void Uebersetzung::init(std::function<void(void)>& display_progress) {
    auto timer = std::chrono::system_clock::now();
    if (uebersetzungen.empty()) {
        for (const auto& sprachordner : std::filesystem::directory_iterator("data")) {
            if (sprachordner.is_directory()) {
                const std::string& sprache = sprachordner.path().filename().string();
                for (const auto& datei : std::filesystem::directory_iterator(sprachordner)) {
                    if (datei.is_regular_file() && datei.path().string().find(".xml") != std::string::npos) {

                        // Datei Puffern
                        if (std::ifstream in(datei.path()); in.good()) {

                            /// Text importieren - OSIS-ID, Text
                            std::cout << "Uebersetzung wird importiert, Sprache: " << sprache << ' ' << datei.path().string() << std::endl;

                            Uebersetzung u;
                            std::string trennchar;
                            const std::string key = datei.path().filename().string();

                            for (std::string s; std::getline(in, s);) {

                                // Ladeanimation
                                if (static unsigned counter = 0; counter++ % 1000 == 0) display_progress();

                                // Vers Start
                                if (s.find("verse") != std::string::npos && s.find("osisID") != std::string::npos) {
                                    if (trennchar.empty()) {
                                        const auto pos1 = s.find('\'');
                                        const auto pos2 = s.find('\"');
                                        trennchar = pos1 < pos2 ? "'" : "\"";
                                    }
                                    std::string osis_id = Sonstiges::get_text_zwischen(s, trennchar, trennchar);
                                    std::string txt = Sonstiges::get_text_zwischen(s, ">", "<");

                                    //std::cout << "\tOSIS-ID: " << osis_id << '\n';
                                    //std::cout << "\t\ttxt: " << txt << std::endl;

                                    // Fehlerbehandlung
                                    if (osis_id.empty()) {
                                        std::cerr << "[Warnung] osis_id leer: " << datei << ' ' << s << '\n';
                                        continue;
                                    }

                                    // Weitere Zeile auslesen
                                    if (txt.empty())  {
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
                                        Sonstiges::replace(s2, "<note>",   "[");
                                        Sonstiges::replace(s2, "</note>",  "]");
                                        for (unsigned limit = 0; limit < 5 &&
                                                s2.find('<') != std::string::npos &&
                                                s2.find('>') != std::string::npos; ++limit)
                                        {
                                            s2.erase(s2.find('<'), s2.find('>'));
                                        }
                                        // Leading Whitespace entfernen
                                        const auto first_char = s2.find_first_not_of(" \t");
                                        if (first_char > 0 && first_char != std::string::npos) s2 = s2.substr(first_char);
                                        txt.append(s2);
                                        if (txt.empty()) std::cerr << "[Warnung] txt leer: s=" << s << ", s2=" << s2 << '\n';
                                    }

                                    // Anmerkungen
                                    Sonstiges::replace(s, "<note>",   "[");
                                    Sonstiges::replace(s, "</note>",  "]");

                                    // Speichern
                                    u.texte[osis_id].append(txt);

                                    // Kapiteleintrag schreiben
                                    const auto& tokens = Sonstiges::tokenize(osis_id, '.');
                                    if (tokens.size() == 3) {
                                        try {
                                            Buch& b = Buch::buecher[tokens[0]];
                                            const unsigned kapitel = std::stoul(tokens[1]);
                                            const unsigned vers    = std::stoul(tokens[2]);
                                            b.pos  = std::get<0>(Buch::get_order(tokens[0]));
                                            b.name = std::get<1>(Buch::get_order(tokens[0]));
                                            b.n_kapitel = std::max(kapitel, b.n_kapitel);
                                            b.n_verse[kapitel] = std::max(vers,    b.n_verse[kapitel]);
                                            if (b.osis_ids[kapitel][vers].empty()) b.osis_ids[kapitel][vers] = osis_id;
                                            else if (b.osis_ids[kapitel][vers] != osis_id) {
                                                std::cerr << "[Warnung] osis_id inkonsistent: ";
                                                std::cerr << osis_id << '/' << b.osis_ids[kapitel][vers] << '\n';
                                            }
                                        } catch (const std::exception& e) {
                                            std::cerr << "[Warnung] osis_id Parse Fehler: " << osis_id << '\n';
                                        }
                                    }
                                    else std::cerr << "[Warnung] osis_id tokens != 3 in " << datei << ' ' << osis_id << '\n';
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
                            std::cout << "\tImport von " << key << " abgeschlossen\n";
                            uebersetzungen[sprache][key] = u;
                        } else std::cerr << "[Warnung] Uebersetzung konnte nicht geoeffnet werden: " << datei.path().string() << '\n';
                    }
                }
            }
        }
        const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - timer
        );
        std::cout << "Ladebenchmark: " << (millis.count() / 1000.f) << 's' << std::endl;
    }
}
