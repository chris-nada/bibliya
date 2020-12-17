#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <imgui-SFML.h>
#include <cmath>
#include "laden.hpp"

Laden::Laden(sf::RenderWindow& window) {
    Laden::window = &window;
}

void Laden::show(std::function<void(std::function<void(void)>&)>& prozess) {
    std::function<void(void)> lade_animation = [&]() {
        static const float r = 100.f;
        static sf::CircleShape form(r, 6);
        static sf::Font font;
        static sf::Text text;
        for (static bool init = true; init; init = false) {
            form.setPosition(window->getSize().x/2 - r/2, window->getSize().y/2 - r/2);
            form.setFillColor({0xA0, 0x80, 0x80});
            form.setOrigin(100.f, 100.f);
            font.loadFromFile("data/RursusCompactMono.ttf");
            text.setFont(font);
            text.setString(L"Texte werden geladen...");
            text.setPosition(window->getSize().x/8, form.getPosition().y);
        }
        window->clear();

        form.rotate(1);
        window->draw(form);
        window->draw(text);

        window->display();
    };
    prozess(lade_animation);
}
