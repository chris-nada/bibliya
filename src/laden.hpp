#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <functional>

/// Ladeanimation.
class Laden final {

public:

    explicit Laden(sf::RenderWindow& window);

    void show(std::function<void(std::function<void(void)>&)>& prozess);

private:

    sf::RenderWindow* window;

};
