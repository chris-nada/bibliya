#pragma once

#include "ui.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>
#include <tuple>

/// Vordeklaration.
class Buch;

/**
 * Hauptansicht.
 *
 * + Menü links = `show_config()` und `ui_verswahl()` und `ui_uebersetzungswahl()`
 * + Menübalken oben = `show_lesezeichen()` und `show_suche()` und `show_einstellungen()` und `show_karte()`
 * + Lesetexte = `show_texte()`
 *
 * @note Mainmenu::show() startet eine Schleife bis das Fenster geschlossen wird.
 */
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

    /// ID Karte
    static inline const char* id_karte = "Karte##win_karte";

public:

    /// Ctor.
    Mainmenu();

    /// Standard-Ctor über SF-Fenster.
    explicit Mainmenu(sf::RenderWindow& window);

    /// Destruktor.
    ~Mainmenu();

    /// Startet die Fensteranzeige (Schleife).
    void show();

private: // UI

    void show_config();

    void show_texte();

    void show_lesezeichen();

    void show_suche();

    void show_einstellungen();

    void show_karte();

    void ui_verswahl();

    void ui_uebersetzungswahl();

    void farben_setzen();

    sf::RenderWindow* window = nullptr;

    /// Loop?
    bool open = true;

    bool open_lesezeichen = false;

    bool open_suche = false;

    bool open_einstellungen = false;

    bool open_karte = false;

private: // Arbeitsdaten

    /// Anzuzeigende Übersetzungen [Sprache] -> Set von [ID].
    std::vector<std::tuple<std::string, std::string>> keys;

    /// Anzuzeigendes Buch.
    const Buch* buch = nullptr;

    /// Anzuzeigendes Kapitel.
    unsigned auswahl_kapitel = 1;

    /// (Erster) anzuzeigender Vers.
    unsigned auswahl_vers = 1;

    /// 0 = Einzeln, 1 = Fünf Verse, 2 = Kapitel.
    int auswahl_modus = 0;

    /// Größe der Schrift (1-6).
    unsigned text_groesse = 3;

    /// Textfarbe.
    ImVec4 farbe_text = ImGui::GetStyleColorVec4(ImGuiCol_Text);

    /// Fensterhintergrundfarbe.
    ImVec4 farbe_hg = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);

    /// Farbe der Versziffern.
    ImVec4 farbe_versziffern = ImColor(UI::FARBE1);

};
