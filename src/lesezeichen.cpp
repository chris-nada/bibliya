#include "lesezeichen.hpp"
#include "buch.hpp"
#include "ui.hpp"
#include "tab.hpp"

#include <fstream>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <imgui.h>

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

void Lesezeichen::show(const char* id_lesezeichen, bool* open_lesezeichen, Tab& aktueller_tab) {
    UI::push_font();
    if (ImGui::Begin(id_lesezeichen, open_lesezeichen)) {

        // X Größe sicherstellen
        if (ImGui::GetWindowSize().x < 580.f) ImGui::SetWindowSize({580.f, ImGui::GetWindowSize().y});

        // Neues Lesezeichen
        static char notiz[0xFF] = "";
        ImGui::Text("%s %u:%u", aktueller_tab.get_buch()->get_name().c_str(), aktueller_tab.auswahl_kapitel, aktueller_tab.auswahl_vers);
        ImGui::SameLine();
        ImGui::InputTextWithHint("##input_notiz", "Notiz", notiz, IM_ARRAYSIZE(notiz));
        ImGui::SameLine();
        UI::push_icons();
        if (ImGui::Button("\uF067")) {
            Lesezeichen l(notiz, aktueller_tab.get_buch()->get_key(), aktueller_tab.auswahl_kapitel, aktueller_tab.auswahl_vers);
            Lesezeichen::add(l);
        }
        ImGui::PopFont();
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Lesezeichen hinzufügen");
        ImGui::NewLine();

        // Alle Lesezeichen auflisten
        for (unsigned i = 0; i < Lesezeichen::alle().size(); ++i) {
            const Lesezeichen& l = Lesezeichen::alle()[i];
            if (Buch::get_buecher().count(l.buch) == 0) continue;
            const Buch& l_buch = Buch::get_buecher().at(l.buch);

            // Löschen
            UI::push_icons();
            if (std::string id("\uF014##l_del_" + std::to_string(i)); ImGui::Button(id.c_str())) {
                Lesezeichen::remove(i);
                ImGui::PopFont();
                break;
            }
            ImGui::PopFont();
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Lesezeichen Entfernen");
            UI::push_icons();

            // Auswählen
            ImGui::SameLine();
            if (std::string id("\uF061##l_goto_" + std::to_string(i)); ImGui::Button(id.c_str())) {
                aktueller_tab.set_buch(&l_buch);
                aktueller_tab.auswahl_kapitel = l.kapitel;
                aktueller_tab.auswahl_vers = l.vers;
                ImGui::PopFont();
                break;
            }
            ImGui::PopFont();
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Anzeigen");

            ImGui::SameLine();
            ImGui::Text("%s %u:%u %s", l_buch.get_name().c_str(), l.kapitel, l.vers, l.notiz.c_str());
        }
    }
    ImGui::End();
    ImGui::PopFont();
}
