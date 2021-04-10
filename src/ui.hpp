#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/WindowStyle.hpp>

/// Hilfsmethoden für die Nutzeroberfläche.
class UI final {

public:

    /// Standardfarbe 1.
    static const inline sf::Color FARBE1 = {0x80, 0x60, 0x60, 0xFF};

    /// Einmalig aufrufen um Schema einzustellen und Schrift(en) zu laden.
    static void setup_style();

    /// Min = 0, Max = 6 und Tatsächliche Größe entspricht `8 + 2 * size`.
    static void push_font(unsigned size = 2);

    /// Pusht die Icons als aktiven Font.
    static void push_icons();

    /// Zeigt ein (?) mit gegebenem `text` als Maus-over an.
    static void tooltip(const char* text);

    /// Welche Fensterart genutzt werden soll.
    static int get_fenstertyp() { return sf::Style::Default; };

};
