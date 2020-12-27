#pragma once

#include "ui.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <unordered_map>
#include <deque>
#include <imgui.h>
#include <unordered_set>

class Buch;

class Mainmenu final {

    /// Horizontale Aufteilung
    static constexpr float FAKTOR_PART1 = 0.15f;

    /// Standardabstand z.B. oberer Rand
    static constexpr float PADDING = 16.f;

    /// ID Lesezeichenfenster
    static inline const char* id_lesezeichen = "Lesezeichen##win_lesezeichen";

    /// ID Suchfenster
    static inline const char* id_suche = "Suche##win_suchen";

    /// ID Einstellungenfenster
    static inline const char* id_einstellungen = "Einstellungen##win_einstellungen";

public:

    /// Ctor.
    Mainmenu();

    /// Standard-Ctor über SF-Fenster.
    explicit Mainmenu(sf::RenderWindow& window);

    /// Destruktor.
    ~Mainmenu();


    void show();

private: // UI

    void show_config();

    void show_texte();

    void show_lesezeichen();

    void show_suche();

    void show_einstellungen();

    void ui_verswahl();

    void ui_uebersetzungswahl();

    void farben_setzen();

    sf::RenderWindow* window;

    /// Loop.
    bool open = true;

    bool open_lesezeichen = false;

    bool open_suche = false;

    bool open_einstellungen = false;

private: // Arbeitsdaten

    /// Anzuzeigende Übersetzungen [Sprache] -> Set von [ID]
    std::unordered_map<std::string, std::unordered_set<std::string>> keys;

    const Buch* buch = nullptr;

    unsigned auswahl_kapitel = 1;

    unsigned auswahl_vers = 1;

    /// 0 = Einzeln, 1 = Fünf Verse, 2 = Kapitel
    int auswahl_modus = 0;

    unsigned text_groesse = 3;

    ImVec4 farbe_text = ImGui::GetStyleColorVec4(ImGuiCol_Text);

    ImVec4 farbe_hg = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);

    ImVec4 farbe_versziffern = ImColor(UI::FARBE1);

};
