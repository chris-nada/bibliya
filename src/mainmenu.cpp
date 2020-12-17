#include "mainmenu.hpp"
#include "ui.hpp"
#include "uebersetzung.hpp"
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
        //

        ImGui::SFML::Render(*window);
        window->display();
        if (!open) window->close();
    }
}

void Mainmenu::show_config() {
    ImGui::SetNextWindowPos({0,0});
    ImGui::SetNextWindowSize({window->getSize().x * FAKTOR_PART1, static_cast<float>(window->getSize().y)});
    ImGui::Begin("##win_konfig", nullptr, WINDOW_FLAGS);

    // Sprachauswahl
    static auto uebersetzungen = Uebersetzung::get_uebersetzungen();
    ImGui::Text("Uebersetzungen gefunden: %d", static_cast<unsigned>(uebersetzungen.size()));

    // Übersetzungsauswahl

    // Hinzufügen

    ImGui::Separator();

    // Buchauswahl

    // Kapitelauswahl

    ImGui::End();
}

void Mainmenu::show_history() {
    ImGui::SetNextWindowPos({window->getSize().x * FAKTOR_PART1, 0});
    ImGui::SetNextWindowSize({
        static_cast<float>(window->getSize().x) - window->getSize().x * FAKTOR_PART1,
        static_cast<float>(window->getSize().y)
    });
    ImGui::Begin("##win_text", &open, WINDOW_FLAGS);

    ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(0xFF00FF00), "%s", "Bla");

    ImGui::End();
}
