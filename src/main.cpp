#include "mainmenu.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>
#include <implot.h>
#include <imgui-SFML.h>

int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Nadabib", sf::Style::None);
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);
    ImPlot::CreateContext();

    Mainmenu menu(window);
    menu.show();

    ImPlot::DestroyContext();
    ImGui::SFML::Shutdown();
}
