#include "mainmenu.hpp"
#include "uebersetzung.hpp"
#include "lesezeichen.hpp"
#include "suche.hpp"

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
            unsigned farbe_hg_temp, farbe_text_temp;
            boa(keys, tabs, text_groesse,
                farbe_hg_temp, farbe_text_temp
            );
            farbe_hg   = ImGui::ColorConvertU32ToFloat4(farbe_hg_temp);
            farbe_text = ImGui::ColorConvertU32ToFloat4(farbe_text_temp);
        } catch (const std::exception& e) {
            std::cerr << "Arbeitsstand konnte nicht importiert werden. Veraltete Version?" << std::endl;
        }
    }
    if (tabs.empty()) tabs.emplace_back();
}

Mainmenu::Mainmenu(sf::RenderWindow& window) : Mainmenu() {
    Mainmenu::window = &window;
}

Mainmenu::~Mainmenu() {
    if (std::ofstream out("data/save.dat", std::ios::binary); out.good()) {
        cereal::PortableBinaryOutputArchive boa(out);
        boa(keys, tabs, text_groesse,
            ImGui::ColorConvertFloat4ToU32(farbe_hg), ImGui::ColorConvertFloat4ToU32(farbe_text)
        );
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
                    case TASTE::Up:     get_tab().auswahl_vers--; break;
                    case TASTE::Down:   get_tab().auswahl_vers++; break;
                    case TASTE::Left:   get_tab().auswahl_kapitel--; break;
                    case TASTE::Right:  get_tab().auswahl_kapitel++; break;
                    case TASTE::Home:   get_tab().auswahl_vers = 1; break;
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
        if (UI::get_fenstertyp() != sf::Style::Default) {
            ImGui::SetCursorPosX(size_x - 40);
            if (ImGui::Button("_##minimieren")) {
                #ifdef __WIN32__
                sf::WindowHandle handle = window->getSystemHandle();
                ShowWindow(handle, SW_MINIMIZE);
                #endif
            }
            if (ImGui::Button("X##schliessen")) window->close();
        }
        ImGui::EndMenuBar();
    }

    // Text(e)
    UI::push_font(text_groesse);
    ImGui::SetCursorPosY(PADDING * 2);

    // Tabs: Start
    ImGui::BeginTabBar("##text_tabs");
    for (unsigned tab_index = 0; tab_index < tabs.size(); ++tab_index) {

        const std::string tab_id(tabs[tab_index].get_name() + "###tabitem_" + std::to_string(tab_index+1));
        const bool begin_tab = ImGui::BeginTabItem(tab_id.c_str());
        const bool close_tab = tab_index > 0 && (ImGui::IsItemClicked(ImGuiMouseButton_Right) ||
                                                 ImGui::IsItemClicked(ImGuiMouseButton_Middle));

        // Tab +
        if (tab_index == tabs.size()-1) if (ImGui::SameLine(); ImGui::Button("+##neues_tab")) tabs.emplace_back(get_tab());

        // Tab schließen?
        if (close_tab) {
            tabs.erase(tabs.begin() + tab_index);
            if (begin_tab) ImGui::EndTabItem();
            continue;
        }
        if (!begin_tab) continue;

        tab = tab_index; // Aktiver Tab

        // Text als Child, damit Tabs nicht weggescrollt werden
        const std::string child_id(tab_id + "child");
        ImGui::BeginChild(child_id.c_str());

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
                if (i < keys.size() - 1) {
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
                const std::string& text = u.get_text(get_tab().get_buch()->get_osis_id(kapitel, vers));
                ImGui::TextColored({farbe_versziffern}, "%u", vers);
                ImGui::TextWrapped("%s", text.c_str());
            };

            // Verse auflisten
            unsigned vers_start = get_tab().auswahl_vers;
            unsigned vers_ende = get_tab().auswahl_vers;
            if (get_tab().auswahl_modus == 1) vers_ende = get_tab().auswahl_vers + 4;
            else if (get_tab().auswahl_modus == 2)
                vers_ende = get_tab().get_buch()->get_n_verse(get_tab().auswahl_kapitel);

            float max_y = ImGui::GetCursorPosY();
            float max_y_new = ImGui::GetCursorPosY();
            for (unsigned v = vers_start;
                 v <= vers_ende && v <= get_tab().get_buch()->get_n_verse(get_tab().auswahl_kapitel); ++v) {
                for (unsigned i = 0; i < ubersetzungen.size(); ++i) {
                    if (i == 0) max_y = max_y_new;
                    ImGui::SetCursorPosY(max_y);
                    const Uebersetzung* u = ubersetzungen[i];
                    verstext(*u, get_tab().auswahl_kapitel, v);
                    max_y_new = std::max(max_y_new, ImGui::GetCursorPosY());
                    ImGui::NextColumn();
                }
            }
            ImGui::Columns();
        }
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
    // Fenster Ende
    ImGui::PopFont();
    ImGui::EndTabBar();
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
                                                    get_tab().get_buch()->get_name().c_str(),
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
            if (get_tab().get_buch()->get_key() == temp_buch.get_key()) {
                ImGui::PushStyleColor(ImGuiCol_FrameBg, {0xFF, 0x00, 0xFF, 0xFF});
                if (ImGui::Selectable(temp_buch.get_name().c_str())) get_tab().set_buch(&temp_buch);
                ImGui::PopStyleColor();
            }
            else if (ImGui::Selectable(temp_buch.get_name().c_str())) get_tab().set_buch(&temp_buch);
        }
        ImGui::EndCombo();
    }

    // Buch -/+
    const unsigned buch_pos_alt = get_tab().get_buch()->get_pos();
    unsigned buch_pos_neu = buch_pos_alt;
    ImGui::SameLine(); if (ImGui::Button("-##buch_minus")) --buch_pos_neu;
    ImGui::SameLine(); if (ImGui::Button("+##buch_plus")) ++buch_pos_neu;
    if (buch_pos_neu != buch_pos_alt && buch_pos_neu >= 1 && buch_pos_neu <= buecher.size()) {
        get_tab().set_buch(&Buch::get_buch(buch_pos_neu));
    }

    // Kapitelauswahl
    ImGui::NewLine();
    static const unsigned STEP = 1;
    ImGui::TextUnformatted("Kapitel");
    if (ImGui::InputScalar("##input_Kapitel", ImGuiDataType_U32, &get_tab().auswahl_kapitel, &STEP, nullptr, "%u")) {
        get_tab().auswahl_vers = 1; // Neues Kapitel -> zu Vers 1
    }
    get_tab().auswahl_kapitel = std::clamp(get_tab().auswahl_kapitel, 1u, get_tab().get_buch()->get_n_kapitel());

    // Versauswahl
    ImGui::NewLine();
    ImGui::TextUnformatted("Vers");
    ImGui::InputScalar("##input_Vers", ImGuiDataType_U32, &get_tab().auswahl_vers, &STEP, nullptr, "%u");
    get_tab().auswahl_vers = std::clamp(get_tab().auswahl_vers, 1u, get_tab().get_buch()->get_n_verse(get_tab().auswahl_kapitel));

    // Modus
    ImGui::NewLine();
    ImGui::TextUnformatted("Darstellung");
    ImGui::RadioButton("1 Vers",  &get_tab().auswahl_modus, 0);
    ImGui::RadioButton("5 Verse", &get_tab().auswahl_modus, 1);
    ImGui::RadioButton("Kapitel", &get_tab().auswahl_modus, 2);
}

void Mainmenu::show_lesezeichen() {
    if (open_lesezeichen) Lesezeichen::show(id_lesezeichen, &open_lesezeichen, get_tab());
}

void Mainmenu::show_suche() {
    if (open_suche) Suche::show(id_suche, &open_suche, get_tab());
}

void Mainmenu::show_einstellungen() {
    if (!open_einstellungen) return;
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

void Mainmenu::show_karte() {
    if (!open_karte) return;
    UI::push_font();
    if (ImGui::Begin(id_karte, &open_karte, ImGuiWindowFlags_HorizontalScrollbar)) {
        static sf::Texture karte;
        if (karte.getSize().x == 0) karte.loadFromFile("data/gfx/palestina.jpg");
        ImGui::Image(karte);
    }
    ImGui::End();
    ImGui::PopFont();
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
