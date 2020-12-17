#include "uebersetzung.hpp"

std::unordered_map<std::string, std::unordered_map<std::string, Uebersetzung>> Uebersetzung::uebersetzungen;

const std::unordered_map<std::string, Buch>& Uebersetzung::get_buecher() {

}

const std::string& Uebersetzung::get_text(const std::string& osis_id) const {
    try { return texte.at(osis_id); }
    catch (const std::exception& e) {
        static std::string nichts = "-";
        return nichts;
    }
}
