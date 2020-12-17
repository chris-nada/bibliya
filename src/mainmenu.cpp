#include "mainmenu.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <filesystem>

Mainmenu::Mainmenu() {
}

Mainmenu::Mainmenu(sf::RenderWindow& window) : Mainmenu() {
    Mainmenu::window = &window;
}

Mainmenu::~Mainmenu() {

}

void Mainmenu::show() {
    sf::Clock deltaClock;
    while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) window->close();
            if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::Escape) window->close();
            }
        }
        ImGui::SFML::Update(*window, deltaClock.restart());
        window->clear();

        show_config();
        show_history();

        // SFML Renders


        ImGui::SFML::Render(*window);
        window->display();
    }
}

void Mainmenu::show_config() {
    if (static bool open = true; open) {
        ImGui::SetNextWindowBgAlpha(0x0);
        ImGui::Begin("win_playground", &open);

        ImGui::End();
    }
}

void Mainmenu::show_history() {
    ImGui::SetNextWindowBgAlpha(BG_ALPHA);
    ImGui::Begin("win_log");
    ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(0xFF00FF00), "%s", "Bla");
    ImGui::End();
}
