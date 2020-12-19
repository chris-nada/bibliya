#include <fstream>
#include <iostream>
#include <mutex>
#include "buch.hpp"
#include "sonstiges.hpp"

std::unordered_map<std::string, Buch> Buch::buecher;

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
    static std::mutex mutex;
    static unsigned pos = 1;
    if (metadaten.empty()) {
        std::lock_guard lock(mutex);
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
        std::lock_guard lock(mutex);
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
