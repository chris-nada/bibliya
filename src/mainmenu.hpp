#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <unordered_map>
#include <deque>
#include <imgui.h>
#include <unordered_set>

class Buch;

class Mainmenu final {

    static constexpr float FAKTOR_PART1 = 0.15f;

    static constexpr float PADDING = 16.f;

    static inline const char* id_lesezeichen = "Lesezeichen##win_lesezeichen";

    static inline const char* id_suche = "Suche##win_suchen";

public:

    Mainmenu();

    explicit Mainmenu(sf::RenderWindow& window);

    ~Mainmenu();

    void show();

private: // UI

    void show_config();

    void show_texte();

    void show_lesezeichen();

    void show_suche();

    void ui_verswahl();

    void ui_uebersetzungswahl();

    sf::RenderWindow* window;

    bool open = true;

    bool open_lesezeichen = false;

    bool open_suche = false;

private: // Arbeitsdaten

    std::unordered_set<std::string> keys;

    const Buch* buch;

    unsigned auswahl_kapitel = 1;

    unsigned auswahl_vers = 1;

    /// 0 = Einzeln, 1 = Fünf Verse, 2 = Kapitel
    int auswahl_modus = 0;

};
