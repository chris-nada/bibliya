#pragma once

#include <imgui.h>

class UI final {

public:

    static const inline sf::Color FARBE1 = {0x80, 0x60, 0x60, 0xFF};

    static void setup_style();

    /// Min = 0, Max = 6 und Tatsächliche Größe entspricht `8 + 2 * size`.
    static void push_font(unsigned size = 2);

    static void tooltip(const char* text);

private:



};
