#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <unordered_map>
#include <deque>

class Mainmenu final {

    static constexpr float BG_ALPHA = 0.2f;

public:

    Mainmenu();

    explicit Mainmenu(sf::RenderWindow& window);

    ~Mainmenu();

    void show();

private:

    void show_config();

    void show_history();

    sf::RenderWindow* window;

};
