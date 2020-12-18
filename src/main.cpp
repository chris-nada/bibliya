#include "mainmenu.hpp"
#include "laden.hpp"
#include "uebersetzung.hpp"
#include "ui.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>
#include <implot.h>
#include <imgui-SFML.h>

int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Nadabib", sf::Style::None);
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window, false);
    ImPlot::CreateContext();
    UI::setup_style();

    std::function<void(std::function<void(void)>&)> init = Uebersetzung::init;
    Laden laden(window);
    laden.show(init);

    Mainmenu menu(window);
    menu.show();

    ImPlot::DestroyContext();
    ImGui::SFML::Shutdown();
}
