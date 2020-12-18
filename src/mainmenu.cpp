#include "mainmenu.hpp"
#include "ui.hpp"
#include "uebersetzung.hpp"
#include <imgui-SFML.h>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <filesystem>

Mainmenu::Mainmenu() {

}

Mainmenu::Mainmenu(sf::RenderWindow& window) : Mainmenu() {
    Mainmenu::window = &window;
}

Mainmenu::~Mainmenu() {

}

void Mainmenu::show() {
    sf::Clock deltaClock;
    while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) window->close();
            if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::Escape) window->close();
            }
        }
        ImGui::SFML::Update(*window, deltaClock.restart());
        window->clear();

        show_config();
        show_history();

        // SFML Renders
        //

        ImGui::SFML::Render(*window);
        window->display();
        if (!open) window->close();
    }
}

void Mainmenu::show_config() {
    ImGui::SetNextWindowPos({0,0});
    ImGui::SetNextWindowSize({window->getSize().x * FAKTOR_PART1, static_cast<float>(window->getSize().y)});
    ImGui::Begin("##win_konfig", nullptr, WINDOW_FLAGS);
    UI::push_font(2);
    ImGui::SetCursorPosY(window->getSize().y / 10.f);

    // Sprachauswahl
    ImGui::NewLine();
    ImGui::TextUnformatted("Sprache");
    static std::vector<std::string> sprachen;
    static unsigned auswahl_sprache = 0;
    if (sprachen.empty()) for (const auto& paar : Uebersetzung::get_uebersetzungen()) {
        sprachen.push_back(paar.first);
    }
    if (ImGui::BeginCombo("##SpracheCombo", sprachen[auswahl_sprache].c_str())) {
        for (unsigned i = 0; i < sprachen.size(); ++i) {
            const bool is_selected = (auswahl_sprache == i);
            if (ImGui::Selectable(sprachen[i].c_str(), is_selected)) auswahl_sprache = i;
            if (is_selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    // Übersetzungsauswahl
    ImGui::NewLine();
    ImGui::TextUnformatted("Übersetzung");
    static const std::unordered_map<std::string, Uebersetzung>* uebersetzungen;
    static unsigned auswahl_uebersetzung = 0;
    uebersetzungen = &Uebersetzung::get_uebersetzungen().at(sprachen.at(auswahl_sprache));
    if (auswahl_uebersetzung > uebersetzungen->size()) auswahl_uebersetzung = 0;
    auto uebersetzung_it = uebersetzungen->cbegin();
    std::advance(uebersetzung_it, auswahl_uebersetzung);
    if (ImGui::BeginCombo("##ÜbersetzungCombo", uebersetzung_it->second.get_name().c_str())) {
        for (unsigned i = 0; i < uebersetzungen->size(); ++i) {
            const bool is_selected = (auswahl_uebersetzung == i);
            auto temp_it = uebersetzungen->cbegin();
            std::advance(temp_it, i);
            if (ImGui::Selectable(temp_it->second.get_name().c_str(), is_selected)) auswahl_uebersetzung = i;
            if (is_selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::TextWrapped("%s", uebersetzung_it->second.get_info().c_str());

    // Hinzufügen
    ImGui::SetCursorPosY(window->getSize().y/2.f);
    if (ImGui::Button("Hinzufügen")) keys.insert(uebersetzung_it->first);

    ImGui::NewLine();
    ImGui::Separator();

    // Buchauswahl
    ImGui::NewLine();
    ImGui::TextUnformatted("Buch");
    static unsigned auswahl_buch = 0;
    static std::vector<std::string> buecher;
    static const auto& buecher_paare = Buch::get_buecher();
    if (buecher.empty()) {
        for (const auto& temp_buch : buecher_paare) buecher.push_back(temp_buch.first);
        std::sort(buecher.begin(), buecher.end(), [](const std::string& lhs, const std::string& rhs) {
            return buecher_paare.at(lhs).get_pos() < buecher_paare.at(rhs).get_pos();
        });
    }
    if (ImGui::BeginCombo("##BuchCombo", buecher_paare.at(buecher[auswahl_buch]).get_name().c_str())) {
        static const ImColor FARBE = {UI::FARBE1};
        static const auto einschub = [](const char* text) {
            ImGui::Separator();
            ImGui::TextColored(FARBE, "%s", text);
            ImGui::Separator();
        };
        bool unbekannt = false;
        einschub( "- Altes Testament -");
        for (unsigned i = 0; i < buecher.size(); ++i) {
            const bool is_selected = (auswahl_buch == i);
            const Buch& temp_buch = buecher_paare.at(buecher[i]);

            // Einschübe
            if (temp_buch.get_name() == buecher[i] && !unbekannt) {
                einschub("- Unzugeordnet -");
                unbekannt = true;
            }
            else if (buecher[i] == "Matt") einschub("- Neues Testament -");

            // Auswahl
            if (ImGui::Selectable(temp_buch.get_name().c_str(), is_selected)) auswahl_buch = i;
            if (is_selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    buch = &buecher_paare.at(buecher[auswahl_buch]);

    // Kapitelauswahl
    ImGui::NewLine();
    static const unsigned STEP = 1;
    ImGui::TextUnformatted("Kapitel");
    ImGui::InputScalar("##input_Kapitel", ImGuiDataType_U32, &auswahl_kapitel, &STEP, nullptr, "%u");
    auswahl_kapitel = std::clamp(auswahl_kapitel, 1u, buch->get_n_kapitel());

    // Versauswahl
    ImGui::NewLine();
    ImGui::TextUnformatted("Vers");
    ImGui::InputScalar("##input_Vers", ImGuiDataType_U32, &auswahl_vers, &STEP, nullptr, "%u");
    auswahl_vers = std::clamp(auswahl_vers, 1u, buch->get_n_verse(auswahl_kapitel));

    ImGui::PopFont();
    ImGui::End();
}

void Mainmenu::show_history() {
    ImGui::SetNextWindowPos({window->getSize().x * FAKTOR_PART1, 0});
    ImGui::SetNextWindowSize({
        static_cast<float>(window->getSize().x) - window->getSize().x * FAKTOR_PART1,
        static_cast<float>(window->getSize().y)
    });
    ImGui::Begin("##win_text", &open, WINDOW_FLAGS);
    UI::push_font(3);
    ImGui::SetCursorPosY(window->getSize().y / 10.f);

    if (!keys.empty()) {
        ImGui::Columns(keys.size(), "##texte", true);
        for (const std::string& key : keys) {
            // In allen Sprachen suchen // TODO suboptimal
            for (const auto& paar : Uebersetzung::get_uebersetzungen()) {
                if (paar.second.count(key) != 0) {
                    const Uebersetzung& u = paar.second.at(key);
                    ImGui::TextUnformatted(u.get_name().c_str());
                    std::string btn_label = "(X)##del" + key;
                    if (ImGui::SameLine(); ImGui::Button(btn_label.c_str())) {
                        keys.erase(key);
                        goto ausgang;
                    }
                    const std::string& text = u.get_text(buch->get_osis_id(auswahl_kapitel, auswahl_vers));
                    ImGui::TextWrapped("%s", text.c_str());
                    ImGui::NextColumn();
                }
            }
        }
        ausgang:
        ImGui::Columns();
    }
    ImGui::PopFont();
    ImGui::End();
}
