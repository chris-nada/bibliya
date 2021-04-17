#include "suche.hpp"

#include "ui.hpp"
#include "lesezeichen.hpp"
#include "uebersetzung.hpp"

#include <imgui.h>

void Suche::show(const char* id_suche, bool* open_suche, Tab& aktueller_tab) {
    UI::push_font();
    if (ImGui::Begin(id_suche, open_suche)) {
        static std::vector<Lesezeichen> ergebnisse;
        static auto scroll_alt = std::make_pair(false, ImGui::GetScrollY());
        if (scroll_alt.first) {
            scroll_alt.first = false;
            ImGui::SetScrollY(scroll_alt.second);
        }

        // X Größe sicherstellen
        if (ImGui::GetWindowSize().x < 580.f) ImGui::SetWindowSize({580.f, ImGui::GetWindowSize().y});

        // Neue Suche
        static char suchbegriff_c_str[0xFF] = "";
        static bool nur_nt = true;
        static std::string suchbegriff;
        const auto suchen = [&]() {
            ergebnisse = Uebersetzung::suche(suchbegriff_c_str, nur_nt);
            suchbegriff = std::string(suchbegriff_c_str);
        };
        if (ImGui::InputTextWithHint("##input_suche", "Suchbegriff",
                                     suchbegriff_c_str, IM_ARRAYSIZE(suchbegriff_c_str),
                                     ImGuiInputTextFlags_EnterReturnsTrue)) {
            suchen();
        }
        ImGui::SameLine();
        UI::push_icons();
        if (ImGui::Button("\uF002##btn_suche_start")) suchen();
        ImGui::PopFont();
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Suche starten");
        UI::tooltip("Die Textsuche beachtet Groß-/Kleinschreibung.");
        ImGui::SameLine();
        ImGui::Checkbox("Nur NT", &nur_nt);
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Nur Neues Testament durchsuchen");

        ImGui::NewLine();

        // Suchergebnisse auflisten
        if (ergebnisse.empty() && !suchbegriff.empty()) ImGui::Text("Keine Ergebnisse für %s", suchbegriff.c_str());
        else for (unsigned i = 0; i < ergebnisse.size(); ++i) {
                const Lesezeichen& l = ergebnisse[i];
                if (Buch::get_buecher().count(l.buch) == 0) continue;
                const Buch& l_buch = Buch::get_buecher().at(l.buch);

                // Auswählen
                UI::push_icons();
                if (std::string id("\uF061##suche_goto_" + std::to_string(i)); ImGui::Button(id.c_str())) {
                    scroll_alt = std::make_pair(true, ImGui::GetScrollY()); // Scrollpos merken, Workaround
                    aktueller_tab.set_buch(&l_buch);
                    aktueller_tab.auswahl_kapitel = l.kapitel;
                    aktueller_tab.auswahl_vers = l.vers;
                    ImGui::PopFont();
                    break;
                }
                ImGui::PopFont();
                if (ImGui::IsItemHovered()) ImGui::SetTooltip("Anzeigen");

                ImGui::SameLine();
                ImGui::Text("%s %u:%u", l_buch.get_name().c_str(), l.kapitel, l.vers);
            }
    }
    ImGui::End();
    ImGui::PopFont();
}
