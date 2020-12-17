#include "buch.hpp"

std::unordered_map<std::string, Buch> Buch::buecher;

const std::string& Buch::get_osis_id(unsigned int kapitel, unsigned int vers) const {
    try { return osis_ids.at(kapitel).at(vers); }
    catch (const std::exception& e) {
        static const std::string nichts = "-";
        return nichts;
    }
}

unsigned Buch::get_n_verse(unsigned int kapitel) const {
    try { return n_verse.at(kapitel); }
    catch (const std::exception& e) { return 0; }
}
