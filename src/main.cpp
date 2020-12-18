#include "mainmenu.hpp"
#include "laden.hpp"
#include "uebersetzung.hpp"
#include "ui.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>
#include <implot.h>
#include <imgui-SFML.h>

#ifdef __WIN32__
#include <windows.h>
#endif

int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Nadabib", sf::Style::None);
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window, false);
    ImPlot::CreateContext();
    UI::setup_style();

    // Konsolenfenster verstecken
    #ifdef NDEBUG
        #ifdef __WIN32__
            HWND hWnd = GetConsoleWindow();
            ShowWindow( hWnd, SW_HIDE );
        #endif
    #endif

    std::function<void(std::function<void(void)>&)> init = Uebersetzung::init;
    Laden laden(window);
    laden.show(init);

    Mainmenu menu(window);
    menu.show();

    ImPlot::DestroyContext();
    ImGui::SFML::Shutdown();
}
