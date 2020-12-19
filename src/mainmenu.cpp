#include "mainmenu.hpp"
#include "ui.hpp"
#include "uebersetzung.hpp"
#include "lesezeichen.hpp"

#include <imgui-SFML.h>
#include <SFML/Window/Event.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/string.hpp>
#include <filesystem>
#include <fstream>

#ifdef __WIN32__
    #include <windows.h>
    #include <winuser.h>
#endif

Mainmenu::Mainmenu() {
    if (std::ifstream in("data/save.dat", std::ios::binary); in.good()) {
        cereal::PortableBinaryInputArchive boa(in);
        unsigned buch_pos;
        boa(keys, buch_pos, auswahl_kapitel, auswahl_vers, auswahl_modus);
        for (const auto& buch_paar : Buch::get_buecher()) if (buch_paar.second.get_pos() == buch_pos) {
            buch = &buch_paar.second;
            break;
        }
    }
    if (buch == nullptr) buch = &Buch::get_buch(1);
}

Mainmenu::Mainmenu(sf::RenderWindow& window) : Mainmenu() {
    Mainmenu::window = &window;
}

Mainmenu::~Mainmenu() {
    if (std::ofstream out("data/save.dat", std::ios::binary); out.good()) {
        cereal::PortableBinaryOutputArchive boa(out);
        boa(keys, buch->get_pos(), auswahl_kapitel, auswahl_vers, auswahl_modus);
    }
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
        show_texte();
        show_lesezeichen();
        show_suche();

        // SFML Renders
        //

        ImGui::SFML::Render(*window);
        window->display();
        if (!open) window->close();
    }
}

void Mainmenu::show_config() {
    static const char* win_id = "##win_konfig";
    ImGui::SetNextWindowPos({0,0});
    ImGui::SetNextWindowSize({window->getSize().x * FAKTOR_PART1, static_cast<float>(window->getSize().y)});
    ImGui::Begin(win_id, nullptr, ImGuiWindowFlags_NoTitleBar);

    UI::push_font();

    ImGui::SetCursorPosY(PADDING);
    ui_uebersetzungswahl();

    // Versauswahl
    ImGui::SetCursorPosY(window->getSize().y / 3.f);
    ImGui::Separator();
    ImGui::NewLine();
    ui_verswahl();

    // Merkzettel
    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();

    ImGui::PopFont();
    ImGui::End();
}

void Mainmenu::show_texte() {
    const float size_x = window->getSize().x - window->getSize().x * FAKTOR_PART1;
    ImGui::SetNextWindowPos({window->getSize().x * FAKTOR_PART1, 0});
    ImGui::SetNextWindowSize({size_x, static_cast<float>(window->getSize().y)});
    static const auto WINDOW_FLAGS = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar;
    ImGui::Begin("##win_text", &open, WINDOW_FLAGS);

    // Menübalken
    UI::push_font(6);
    const bool begin_menu_bar = ImGui::BeginMenuBar();
    ImGui::PopFont();
    if (begin_menu_bar) {
        UI::push_icons();
        if (ImGui::Button("\uF02E##Lesezeichen")) {
            ImGui::SetWindowFocus(id_lesezeichen);
            open_lesezeichen = true;
        }
        ImGui::PopFont();
        UI::push_font();
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Lesezeichen");
        ImGui::PopFont();

        UI::push_icons();
        if (ImGui::Button("\uF002##Suche")) {
            ImGui::SetWindowFocus(id_suche);
            open_suche = true;
        }
        ImGui::PopFont();
        UI::push_font();
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Suche");
        ImGui::PopFont();

        ImGui::SetCursorPosX(size_x - 40);
        if (ImGui::Button("_##minimieren")) {
            #ifdef __WIN32__
            sf::WindowHandle handle = window->getSystemHandle();
            ShowWindow(handle, SW_MINIMIZE);
            #endif
        }
        if (ImGui::Button("X##schliessen")) {
            window->close();
        }
        ImGui::EndMenuBar();
    }

    // Text(e)
    UI::push_font(3);
    ImGui::SetCursorPosY(PADDING * 2);
    if (!keys.empty()) {
        ImGui::Columns(keys.size(), "##texte", true);
        for (const std::string& key : keys) {
            // In allen Sprachen suchen // TODO suboptimal
            for (const auto& paar : Uebersetzung::get_uebersetzungen()) {
                if (paar.second.count(key) != 0) {
                    // Entfernen (X)
                    if (std::string btn_label = "X##del" + key; ImGui::Button(btn_label.c_str())) {
                        keys.erase(key);
                        goto ausgang;
                    }
                    UI::push_font();
                    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Übersetzung aus Ansicht entfernen");
                    ImGui::PopFont();

                    // Überschrift = Name d. Übersetzung
                    const Uebersetzung& u = paar.second.at(key);
                    ImGui::SameLine();
                    ImGui::TextUnformatted(u.get_name().c_str());
                    ImGui::NewLine();

                    // Textdarstellung
                    auto verstext = [&](unsigned kapitel, unsigned vers) {
                        const std::string& text = u.get_text(buch->get_osis_id(kapitel, vers));
                        ImGui::TextColored(UI::FARBE1, "%u", vers);
                        ImGui::TextWrapped("%s", text.c_str());
                    };

                    // Verse auflisten
                    unsigned vers_start = auswahl_vers;
                    unsigned vers_ende  = auswahl_vers;
                    if      (auswahl_modus == 1) vers_ende = auswahl_vers + 4;
                    else if (auswahl_modus == 2) vers_ende = buch->get_n_verse(auswahl_kapitel);
                    for (unsigned v = vers_start; v <= vers_ende && v <= buch->get_n_verse(auswahl_kapitel); ++v) {
                        verstext(auswahl_kapitel, v);
                    }
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

void Mainmenu::ui_uebersetzungswahl() {
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
            const std::string& info = temp_it->second.get_info();
            //if (is_selected) ImGui::SetItemDefaultFocus();
            if (!info.empty() && ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Dummy({600.f, 0});
                ImGui::TextWrapped("%s", info.c_str());
                ImGui::EndTooltip();
            }
        }
        ImGui::EndCombo();
    }
    // Hinzufügen
    ImGui::SameLine();
    UI::push_icons();
    if (ImGui::Button("\uF067##hinzufuegen")) keys.insert(uebersetzung_it->first);
    ImGui::PopFont();
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Übersetzung hinzufügen");
}

void Mainmenu::ui_verswahl() {
    // Buchauswahl
    ImGui::TextUnformatted("Buch");
    static std::vector<std::string> buecher;
    static const auto& buecher_paare = Buch::get_buecher();
    if (buecher.empty()) {
        for (const auto& temp_buch : buecher_paare) buecher.push_back(temp_buch.first);
        std::sort(buecher.begin(), buecher.end(), [](const std::string& lhs, const std::string& rhs) {
            return buecher_paare.at(lhs).get_pos() < buecher_paare.at(rhs).get_pos();
        });
    }

    // Buch ComboBox Begin
    const bool begin_buch_combo = ImGui::BeginCombo("##BuchCombo", buch->get_name().c_str());
    if (begin_buch_combo) {
        static const ImColor FARBE = {UI::FARBE1};
        static const auto einschub = [](const char* text) {
            ImGui::Separator();
            ImGui::TextColored(FARBE, "%s", text);
            ImGui::Separator();
        };
        bool start_unbekannte = false;

        // Bücher auflisten
        einschub( "- Altes Testament -");
        for (const auto& buch_key : buecher) {
            const Buch& temp_buch = buecher_paare.at(buch_key);
            const bool is_selected = buch == &temp_buch;

            // Einschübe
            if (temp_buch.get_name() == buch_key && !start_unbekannte) {
                einschub("- Unzugeordnet -");
                start_unbekannte = true;
            }
            else if (buch_key == "Matt") einschub("- Neues Testament -");

            // Auswahl
            if (ImGui::Selectable(temp_buch.get_name().c_str())) buch = &temp_buch;
            if (is_selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    // Buch -/+
    const unsigned buch_pos_alt = buch->get_pos();
    unsigned buch_pos_neu = buch_pos_alt;
    ImGui::SameLine(); if (ImGui::Button("-##buch_minus")) --buch_pos_neu;
    ImGui::SameLine(); if (ImGui::Button("+##buch_plus")) ++buch_pos_neu;
    if (buch_pos_neu != buch_pos_alt && buch_pos_neu >= 1 && buch_pos_neu <= buecher.size()) {
        buch = &Buch::get_buch(buch_pos_neu);
    }

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

    // Modus
    ImGui::NewLine();
    ImGui::TextUnformatted("Darstellung");
    ImGui::RadioButton("1 Vers",  &auswahl_modus, 0);
    ImGui::RadioButton("5 Verse", &auswahl_modus, 1);
    ImGui::RadioButton("Kapitel", &auswahl_modus, 2);
}

void Mainmenu::show_lesezeichen() {
    if (open_lesezeichen) {
        UI::push_font();
        if (ImGui::Begin(id_lesezeichen, &open_lesezeichen)) {

            // X Größe sicherstellen
            if (ImGui::GetWindowSize().x < 580.f) ImGui::SetWindowSize({580.f, ImGui::GetWindowSize().y});

            // Neues Lesezeichen
            static char notiz[0xFF] = "";
            ImGui::Text("%s %u:%u", buch->get_name().c_str(), auswahl_kapitel, auswahl_vers);
            ImGui::SameLine();
            ImGui::InputTextWithHint("##input_notiz", "Notiz", notiz, IM_ARRAYSIZE(notiz));
            ImGui::SameLine();
            UI::push_icons();
            if (ImGui::Button("\uF067")) {
                Lesezeichen l(notiz, buch->get_key(), auswahl_kapitel, auswahl_vers);
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
                    buch = &l_buch;
                    auswahl_kapitel = l.kapitel;
                    auswahl_vers = l.vers;
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
}

void Mainmenu::show_suche() {
    if (open_suche) {
        UI::push_font();
        if (ImGui::Begin(id_suche, &open_suche)) {

            static std::vector<Lesezeichen> ergebnisse;

            // X Größe sicherstellen
            if (ImGui::GetWindowSize().x < 580.f) ImGui::SetWindowSize({580.f, ImGui::GetWindowSize().y});

            // Neue Suche
            static char notiz[0xFF] = "";
            static bool nur_nt = true;
            if (ImGui::InputTextWithHint("##input_suche", "Suchbegriff", notiz, IM_ARRAYSIZE(notiz), ImGuiInputTextFlags_EnterReturnsTrue)) {
                ergebnisse = Uebersetzung::suche(notiz, nur_nt);
            }
            ImGui::SameLine();
            UI::push_icons();
            if (ImGui::Button("\uF002##btn_suche_start")) ergebnisse = Uebersetzung::suche(notiz, nur_nt);
            ImGui::PopFont();
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Suche starten");
            UI::tooltip("Die Textsuche beachtet Groß-/Kleinschreibung.");
            ImGui::SameLine();
            ImGui::Checkbox("Nur NT", &nur_nt);
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Nur Neues Testament durchsuchen");

            ImGui::NewLine();

            // Suchergebnisse auflisten
            for (unsigned i = 0; i < ergebnisse.size(); ++i) {
                const Lesezeichen& l = ergebnisse[i];
                if (Buch::get_buecher().count(l.buch) == 0) continue;
                const Buch& l_buch = Buch::get_buecher().at(l.buch);

                // Auswählen
                UI::push_icons();
                if (std::string id("\uF061##suche_goto_" + std::to_string(i)); ImGui::Button(id.c_str())) {
                    buch = &l_buch;
                    auswahl_kapitel = l.kapitel;
                    auswahl_vers = l.vers;
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
}
