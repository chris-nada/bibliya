#include "tab.hpp"

const Buch* Tab::get_buch() const {
    if (buch == nullptr) buch = &Buch::get_buch(buch_pos);
    return buch;
}

std::string Tab::get_name() const {
    std::string name(get_buch()->get_name());
    if (auswahl_modus != 2 || auswahl_vers > 1) {
        name.reserve(name.size() + 6);
        name.append(' ' + std::to_string(auswahl_kapitel) + ':' + std::to_string(auswahl_vers));
    }
    return name;
}

void Tab::set_buch(const Buch* buch) {
    this->buch = buch;
    if (buch) buch_pos = buch->get_pos();
}
