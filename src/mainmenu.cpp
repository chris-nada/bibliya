#include "mainmenu.hpp"
#include "uebersetzung.hpp"
#include "lesezeichen.hpp"

#include <imgui-SFML.h>
#include <SFML/Window/Event.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>
#include <filesystem>
#include <fstream>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#ifdef __WIN32__
    #include <windows.h>
    #include <winuser.h>
#endif

Mainmenu::Mainmenu() {
    if (std::ifstream in("data/save.dat", std::ios::binary); in.good()) {
        try {
            cereal::PortableBinaryInputArchive boa(in);
            unsigned buch_pos, farbe_hg_temp, farbe_text_temp;
            boa(keys, buch_pos, auswahl_kapitel, auswahl_vers, auswahl_modus, text_groesse,
                farbe_hg_temp, farbe_text_temp);
            farbe_hg   = ImGui::ColorConvertU32ToFloat4(farbe_hg_temp);
            farbe_text = ImGui::ColorConvertU32ToFloat4(farbe_text_temp);
            for (const auto& buch_paar : Buch::get_buecher()) {
                if (buch_paar.second.get_pos() == buch_pos) {
                    buch = &buch_paar.second;
                    break;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Arbeitsstand konnte nicht importiert werden. Veraltete Version?" << std::endl;
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
        boa(keys, buch->get_pos(), auswahl_kapitel, auswahl_vers, auswahl_modus, text_groesse,
            ImGui::ColorConvertFloat4ToU32(farbe_hg), ImGui::ColorConvertFloat4ToU32(farbe_text));
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
                switch (event.key.code) {
                    using TASTE = sf::Keyboard;
                    case TASTE::Escape: window->close(); break;
                    case TASTE::Up:     auswahl_vers--; break;
                    case TASTE::Down:   auswahl_vers++; break;
                    case TASTE::Left:   auswahl_kapitel--; break;
                    case TASTE::Right:  auswahl_kapitel++; break;
                    case TASTE::Home:   auswahl_vers = 1; break;
                    default: break;
                }
            }
        }
        ImGui::SFML::Update(*window, deltaClock.restart());
        window->clear();

        farben_setzen();
        show_config();
        show_texte();
        show_lesezeichen();
        show_suche();
        show_karte();
        show_einstellungen();

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

    ImGui::PopFont();
    ImGui::End();
}

void Mainmenu::show_texte() {
    static const auto WINDOW_FLAGS = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar;
    const float size_x = window->getSize().x - window->getSize().x * FAKTOR_PART1;
    ImGui::SetNextWindowPos({window->getSize().x * FAKTOR_PART1, 0});
    ImGui::SetNextWindowSize({size_x, static_cast<float>(window->getSize().y)});
    ImGui::Begin("##win_text", &open, WINDOW_FLAGS);

    // Menübalken
    UI::push_font(6);
    const bool begin_menu_bar = ImGui::BeginMenuBar();
    ImGui::PopFont();
    if (begin_menu_bar) {

        auto add_ribbon = [](const char* btn_id, const char* window_id, bool& open, const char* tooltip) {
            UI::push_icons();
            if (ImGui::Button(btn_id)) {
                ImGui::SetWindowFocus(window_id);
                open = true;
            }
            ImGui::PopFont();
            if (ImGui::IsItemHovered()) {
                UI::push_font();
                ImGui::SetTooltip("%s", tooltip);
                ImGui::PopFont();
            }
        };

        add_ribbon("\uF02E##R_Lesezeichen", id_lesezeichen, open_lesezeichen, "Lesezeichen");
        add_ribbon("\uF002##R_Suche", id_suche, open_suche, "Suche");
        add_ribbon("\uF279##R_Karte", id_karte, open_karte, "Karte");
        add_ribbon("\uF013##R_Einstellungen", id_einstellungen, open_einstellungen, "Einstellungen");

        // Oben Rechts: (_) und (X)
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
    UI::push_font(text_groesse);
    ImGui::SetCursorPosY(PADDING * 2);

    // Spalten
    if (!keys.empty()) {
        ImGui::Columns(keys.size(), "##texte", true);
        std::vector<const Uebersetzung*> ubersetzungen;

        // Übersetzungen in Vektor schreiben
        for (unsigned i = 0; i < keys.size(); ++i) { // [lang_key] [übersetzungs-key]
            const auto& key = keys[i];
            const Uebersetzung* u = &Uebersetzung::get_uebersetzungen().at(std::get<0>(key)).at(std::get<1>(key));

            // Entfernen (X)
            const std::string id(std::get<0>(key) + std::get<1>(key));
            UI::push_icons();
            if (const std::string btn_label = "\uF00D##del" + id; ImGui::Button(btn_label.c_str())) {
                keys.erase(keys.begin() + i);
                ImGui::PopFont();
                goto ausgang;
            }
            ImGui::PopFont();
            UI::push_font();
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Übersetzung aus Ansicht entfernen");
            ImGui::PopFont();

            // Sortieren <
            if (i > 0) {
                ImGui::SameLine();
                UI::push_icons();
                if (const std::string btn_label("\uF053##<" + id); ImGui::Button(btn_label.c_str())) {
                    std::iter_swap(keys.begin() + i, keys.begin() + i - 1);
                }
                ImGui::PopFont();
            }

            // Sortieren >
            if (i < keys.size()-1) {
                ImGui::SameLine();
                UI::push_icons();
                if (const std::string btn_label("\uF054##>" + id); ImGui::Button(btn_label.c_str())) {
                    std::iter_swap(keys.begin() + i, keys.begin() + i + 1);
                }
                ImGui::PopFont();
            }

            // Überschriften
            ImGui::SameLine();
            ImGui::TextUnformatted(u->get_name().c_str());
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", u->get_name().c_str());
            ImGui::NewLine();
            ImGui::NextColumn();

            ubersetzungen.push_back(u);
        }
        ausgang:

        // Textdarstellungsfunktion
        auto verstext = [&](const Uebersetzung& u, unsigned kapitel, unsigned vers) {
            const std::string& text = u.get_text(buch->get_osis_id(kapitel, vers));
            ImGui::TextColored({farbe_versziffern}, "%u", vers);
            ImGui::TextWrapped("%s", text.c_str());
        };

        // Verse auflisten
        unsigned vers_start = auswahl_vers;
        unsigned vers_ende = auswahl_vers;
        if (auswahl_modus == 1) vers_ende = auswahl_vers + 4;
        else if (auswahl_modus == 2) vers_ende = buch->get_n_verse(auswahl_kapitel);

        float max_y = ImGui::GetCursorPosY();
        float max_y_new = ImGui::GetCursorPosY();
        for (unsigned v = vers_start; v <= vers_ende && v <= buch->get_n_verse(auswahl_kapitel); ++v) {
            for (unsigned i = 0; i < ubersetzungen.size(); ++i) {
                if (i == 0) max_y = max_y_new;
                ImGui::SetCursorPosY(max_y);
                const Uebersetzung* u = ubersetzungen[i];
                verstext(*u, auswahl_kapitel, v);
                max_y_new = std::max(max_y_new, ImGui::GetCursorPosY());
                ImGui::NextColumn();
            }
        }
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
    static unsigned auswahl_uebersetzung = 0;
    if (sprachen.empty()) for (const auto& paar : Uebersetzung::get_uebersetzungen()) {
        sprachen.push_back(paar.first);
    }
    if (ImGui::BeginCombo("##SpracheCombo", sprachen[auswahl_sprache].c_str())) {
        for (unsigned i = 0; i < sprachen.size(); ++i) {
            const bool is_selected = (auswahl_sprache == i);
            if (ImGui::Selectable(sprachen[i].c_str(), is_selected)) {
                auswahl_sprache = i;
                auswahl_uebersetzung = 0;
            }
            if (is_selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    // Übersetzungsauswahl
    ImGui::NewLine();
    ImGui::TextUnformatted("Übersetzung");
    static const std::unordered_map<std::string, Uebersetzung>* uebersetzungen;
    uebersetzungen = &Uebersetzung::get_uebersetzungen().at(sprachen.at(auswahl_sprache));
    if (auswahl_uebersetzung > uebersetzungen->size()) auswahl_uebersetzung = 0;
    auto uebersetzung_it = uebersetzungen->cbegin();
    std::advance(uebersetzung_it, auswahl_uebersetzung);
    if (ImGui::BeginCombo("##ÜbersetzungCombo", uebersetzung_it->second.get_name().c_str())) {
        for (unsigned i = 0; i < uebersetzungen->size(); ++i) {
            const bool is_selected = (auswahl_uebersetzung == i);
            auto temp_it = uebersetzungen->cbegin();
            std::advance(temp_it, i);
            std::string select_id = temp_it->second.get_name() + "##u_sel_" + sprachen[auswahl_sprache] + temp_it->first;
            if (ImGui::Selectable(select_id.c_str(), is_selected)) auswahl_uebersetzung = i;
            const std::string& info = temp_it->second.get_info();
            if (is_selected) ImGui::SetItemDefaultFocus();
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
    if (ImGui::Button("\uF067##uebersetzung_plus")) {
        const auto paar = std::make_tuple(sprachen.at(auswahl_sprache), uebersetzung_it->first);
        if (std::find(keys.begin(), keys.end(), paar) == keys.end()) keys.push_back(paar);
    }
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
    const bool begin_buch_combo = ImGui::BeginCombo("##BuchCombo",
                                                    buch->get_name().c_str(),
                                                    ImGuiComboFlags_HeightLarge);
    if (begin_buch_combo) {
        if (ImGui::IsItemClicked()) std::cout << "Combo open!" << std::endl;
        static const auto einschub = [](const char* text) {
            ImGui::Separator();
            ImGui::TextColored({UI::FARBE1}, "%s", text);
            ImGui::Separator();
        };
        bool start_unbekannte = false;

        // Bücher auflisten
        einschub( "- Altes Testament -");
        for (const auto& buch_key : buecher) {
            const Buch& temp_buch = buecher_paare.at(buch_key);

            // Einschübe
            if (temp_buch.get_name() == buch_key && !start_unbekannte) {
                einschub("- Unzugeordnet -");
                start_unbekannte = true;
            }
            else if (buch_key == "Matt") einschub("- Neues Testament -");

            // Auswahl
            if (buch == &temp_buch) {
                ImGui::PushStyleColor(ImGuiCol_FrameBg, {0xFF, 0x00, 0xFF, 0xFF});
                if (ImGui::Selectable(temp_buch.get_name().c_str())) buch = &temp_buch;
                ImGui::PopStyleColor();
            }
            else if (ImGui::Selectable(temp_buch.get_name().c_str())) buch = &temp_buch;
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
    if (ImGui::InputScalar("##input_Kapitel", ImGuiDataType_U32, &auswahl_kapitel, &STEP, nullptr, "%u")) {
        auswahl_vers = 1; // Neues Kapitel -> zu Vers 1
    }
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

void Mainmenu::show_einstellungen() {
    if (open_einstellungen) {
        UI::push_font();
        if (ImGui::Begin(id_einstellungen, &open_einstellungen)) {
            // Schriftgröße
            static const unsigned STEP = 1;
            static const unsigned WIDTH = 160;
            ImGui::SetNextItemWidth(WIDTH);
            ImGui::InputScalar("Textgröße##input_schriftgroesse", ImGuiDataType_U32, &text_groesse, &STEP, nullptr, "%u");
            text_groesse = std::clamp(text_groesse, 1u, 6u);

            // Farben
            const auto FLAGS = ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoAlpha;
            ImGui::NewLine();
            ImGui::SetNextItemWidth(WIDTH);
            ImGui::ColorPicker3("Textfarbe", reinterpret_cast<float*>(&farbe_text), FLAGS);
            ImGui::NewLine();
            ImGui::SetNextItemWidth(WIDTH);
            ImGui::ColorPicker3("Hintergrundfarbe", reinterpret_cast<float*>(&farbe_hg), FLAGS);
        }
        ImGui::End();
        ImGui::PopFont();
    }
}

void Mainmenu::show_karte() {

    if (open_karte) {
        UI::push_font();
        if (ImGui::Begin(id_karte, &open_karte, ImGuiWindowFlags_HorizontalScrollbar)) {
            static sf::Texture karte;
            if (karte.getSize().x == 0) karte.loadFromFile("data/gfx/palestina.jpg");
            ImGui::Image(karte);
        }
        ImGui::End();
        ImGui::PopFont();
    }
}

void Mainmenu::farben_setzen() {
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = farbe_hg;
    ImGui::GetStyle().Colors[ImGuiCol_Text] = farbe_text;
    ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrab] = farbe_text;
    ImGui::GetStyle().Colors[ImGuiCol_ScrollbarBg] = farbe_hg;
    farbe_versziffern = {
            UI::FARBE1.r / 255.f + farbe_text.x * 0.5f,
            UI::FARBE1.g / 255.f + farbe_text.y * 0.5f,
            UI::FARBE1.b / 255.f + farbe_text.z * 0.5f,
            1.f
    };
}
