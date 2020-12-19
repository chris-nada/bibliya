#include "lesezeichen.hpp"
#include "buch.hpp"

#include <fstream>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

std::vector<Lesezeichen> Lesezeichen::lesezeichen;

Lesezeichen::Lesezeichen(const std::string& notiz, const std::string& buch, unsigned int kapitel, unsigned int vers)
        : notiz(notiz), buch(buch), kapitel(kapitel), vers(vers)
{
    //
}

void Lesezeichen::save() {
    if (std::ofstream out("data/merkzettel.dat", std::ios::binary); out.good()) {
        cereal::PortableBinaryOutputArchive boa(out);
        boa(lesezeichen);
    }
}

std::vector <Lesezeichen>& Lesezeichen::alle() {
    static bool importiert = false;
    if (!importiert && lesezeichen.empty()) {
        if (std::ifstream in("data/merkzettel.dat", std::ios::binary); in.good()) {
            cereal::PortableBinaryInputArchive bin(in);
            bin(lesezeichen);
        }
    }
    return lesezeichen;
}

void Lesezeichen::add(const Lesezeichen& neu) {
    lesezeichen.push_back(neu);
    save();
}

void Lesezeichen::remove(std::size_t pos) {
    if (pos < lesezeichen.size()) lesezeichen.erase(lesezeichen.begin() + pos);
    save();
}

bool Lesezeichen::operator<(const Lesezeichen& rhs) const {
    const auto pos1 = Buch::get_buecher().at(buch).get_pos();
    const auto pos2 = Buch::get_buecher().at(rhs.buch).get_pos();
    if (pos1 < pos2) return true;
    if (pos2 < pos1) return false;
    if (kapitel < rhs.kapitel) return true;
    if (rhs.kapitel < kapitel) return false;
    if (vers < rhs.vers) return true;
    if (rhs.vers < vers) return false;
    return notiz < rhs.notiz;
}
