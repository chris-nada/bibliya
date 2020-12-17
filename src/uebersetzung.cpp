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
    if (uebersetzungen.empty()) {
        for (const auto& sprachordner : std::filesystem::directory_iterator("data")) {
            if (sprachordner.is_directory()) {
                const std::string& sprache = sprachordner.path().filename().string();
                for (const auto& datei : std::filesystem::directory_iterator(sprachordner)) {
                    if (datei.is_regular_file() && datei.path().extension().string().find("xml") != std::string::npos) {
                        if (std::ifstream in(datei.path()); in.good()) {

                            /// Text importieren - OSIS-ID, Text
                            std::cout << "Uebersetzung wird importiert, Sprache: " << sprache << ' ' << datei.path().string() << std::endl;

                            Uebersetzung u;
                            std::string key;

                            std::string osis_id;
                            std::string txt;

                            for (std::string s; std::getline(in, s);) {

                                // Vers Start
                                if (s.find("verse") != std::string::npos && s.find("osisID") != std::string::npos) {
                                    osis_id = Sonstiges::get_text_zwischen(s, "\"", "\"");
                                    txt.append(s.substr('>'));

                                    std::cout << "\tOSIS-ID: " << osis_id << '\n';
                                    std::cout << "\t\ttxt: " << txt << '\n';
                                    u.texte[osis_id].append(txt);

                                    txt.clear();
                                    break;
                                } else if (s.find("ntitel") != std::string::npos) {
                                    u.name = Sonstiges::get_text_zwischen(s);
                                } else if (s.find("ninfo") != std::string::npos) {
                                    u.info = Sonstiges::get_text_zwischen(s);
                                }
                            }

                            // Speichern
                            std::cout << "\tnKey: " << key << '\n';
                            std::cout << "\tnTitel: " << u.name << '\n';
                            std::cout << "\tnInfo: "  << u.info << '\n';
                            uebersetzungen[sprache][key] = u;
                        } else std::cerr << "Uebersetzung konnte nicht geoeffnet werden: " << datei.path().string() << std::endl;
                    }
                }
            }
        }
    }
    return uebersetzungen;
}

const std::unordered_map<std::string, Buch>& Uebersetzung::get_buecher() {
    if (buecher.empty()) {

    }
    return buecher;
}
