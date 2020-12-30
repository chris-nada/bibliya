#include <fstream>
#include <iostream>
#include <tbb/tbb.h>

#include "buch.hpp"
#include "sonstiges.hpp"

std::map<std::string, Buch> Buch::buecher;

const std::string& Buch::get_osis_id(unsigned kapitel, unsigned vers) const {
    try { return osis_ids.at(kapitel).at(vers); }
    catch (const std::exception& e) {
        static const std::string nichts;
        return nichts;
    }
}

unsigned Buch::get_n_verse(unsigned kapitel) const {
    try { return n_verse.at(kapitel); }
    catch (const std::exception& e) { return 1; }
}

const std::tuple<unsigned, std::string>& Buch::get_order(const std::string& id) {
    typedef std::tuple<unsigned, std::string> tuple;
    static std::unordered_map<std::string, tuple> metadaten;
    static tbb::mutex mutex;
    static unsigned pos = 1;
    if (metadaten.empty()) {
        tbb::mutex::scoped_lock lock(mutex);
        if (metadaten.empty()) {
            if (std::ifstream in("data/buecher.ini"); in.good()) {
                for (std::string s; std::getline(in, s);) {
                    if (s.size() >= 3 && s.find('=') != std::string::npos) {
                        const auto& tokens = Sonstiges::tokenize(s, '=');
                        if (tokens.size() != 2 || tokens[0].empty() || tokens[1].empty()) continue;
                        metadaten[tokens[1]] = std::make_tuple(pos, tokens[0]);
                        pos++;
                    }
                }
            }
        }
    }
    if (metadaten.count(id) == 0) {
        tbb::mutex::scoped_lock lock(mutex);
        if (metadaten.count(id) == 0) {
            metadaten[id] = std::make_tuple(pos, id);
            ++pos;
        }
    }
    return metadaten.at(id);
}

const Buch& Buch::get_buch(unsigned int pos) {
    for (const auto& paar : buecher) if (paar.second.get_pos() == pos) return paar.second;
    return buecher.begin()->second;
}

void Buch::buch_osis_check(const std::string& osis_id, unsigned int kapitel, unsigned int vers) {
    // Buch anlegen?
    static tbb::mutex buch_mutex;
    if (Buch::buecher.count(osis_id) == 0) {
        tbb::mutex::scoped_lock lock(buch_mutex);
        Buch::buecher[osis_id];
        Buch& b = Buch::buecher.at(osis_id);
        b.key = osis_id;
        b.pos = std::get<0>(Buch::get_order(osis_id));
        b.name = std::get<1>(Buch::get_order(osis_id));
    }
    Buch& b = Buch::buecher.at(osis_id);
    if (b.n_kapitel < kapitel) {
        tbb::mutex::scoped_lock lock(buch_mutex);
        b.n_kapitel = kapitel;
    }
    if (b.get_n_verse(kapitel) < vers) {
        tbb::mutex::scoped_lock lock(buch_mutex);
        b.n_verse[kapitel] = vers;
    }
    if (b.get_osis_id(kapitel, vers).empty()) {
        tbb::mutex::scoped_lock lock(buch_mutex);
        b.osis_ids[kapitel][vers] = osis_id + "." + std::to_string(kapitel) + "." + std::to_string(vers);
    }
}
