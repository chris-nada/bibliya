#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <unordered_map>
#include <deque>
#include <imgui.h>
#include <unordered_set>

class Buch;

class Mainmenu final {

    static constexpr float FAKTOR_PART1 = 0.15f;

    static constexpr auto WINDOW_FLAGS = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

public:

    Mainmenu();

    explicit Mainmenu(sf::RenderWindow& window);

    ~Mainmenu();

    void show();

private: // UI

    void show_config();

    void show_history();

    void versauswahl();

    sf::RenderWindow* window;

    bool open = true;

private: // Arbeitsdaten

    std::unordered_set<std::string> keys;

    const Buch* buch;

    unsigned auswahl_kapitel = 1;

    unsigned auswahl_vers = 1;

};
