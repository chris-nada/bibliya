#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <unordered_map>
#include <deque>
#include <imgui.h>

class Mainmenu final {

    static constexpr float FAKTOR_PART1 = 0.15f;

    static constexpr auto WINDOW_FLAGS = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

public:

    Mainmenu();

    explicit Mainmenu(sf::RenderWindow& window);

    ~Mainmenu();

    void show();

private:

    void show_config();

    void show_history();

    sf::RenderWindow* window;

    bool open = true;

};
