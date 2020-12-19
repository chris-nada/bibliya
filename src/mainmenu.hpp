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

public:

    Mainmenu();

    explicit Mainmenu(sf::RenderWindow& window);

    ~Mainmenu();

    void show();

private: // UI

    void show_config();

    void show_texte();

    void ui_verswahl();

    void ui_uebersetzungswahl();

    void show_lesezeichen();

    sf::RenderWindow* window;

    bool open = true;

    bool open_lesezeichen = false;

private: // Arbeitsdaten

    std::unordered_set<std::string> keys;

    const Buch* buch;

    unsigned auswahl_kapitel = 1;

    unsigned auswahl_vers = 1;

    /// 0 = Einzeln, 1 = Fünf Verse, 2 = Kapitel
    int auswahl_modus = 0;

};
