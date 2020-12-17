#pragma once

class UI final {

public:

    static void setup_style();

    /// Min = 0, Max = 6 und Tatsächliche Größe entspricht `8 + 2 * size`.
    static void push_font(unsigned size = 2);

    static void tooltip(const char* text);

private:



};
