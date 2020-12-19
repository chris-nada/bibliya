#include <iostream>
#include <imgui-SFML.h>
#include "ui.hpp"
#include "imgui.h"

void UI::setup_style() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsLight(&style);
    style.ChildRounding     = 0;
    style.FrameRounding     = 0;
    style.GrabRounding      = 0;
    style.PopupRounding     = 0;
    style.ScrollbarRounding = 0;
    style.TabRounding       = 0;
    style.WindowRounding    = 0;

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    for (unsigned i = 12; i <= 24; i += 2) {
        io.Fonts->AddFontFromFileTTF(
                "data/RursusCompactMono.ttf", static_cast<float>(i), nullptr, io.Fonts->GetGlyphRangesCyrillic()
         );
    }

    // Icons laden
    ImFontConfig config;
    config.GlyphMinAdvanceX = 14.0f; // Use if you want to make the icon monospaced
    static const ImWchar icon_ranges[] = {0xf000, 0xf2e0, 0};
    io.Fonts->AddFontFromFileTTF("data/icons/fontawesome-webfont.ttf", 14.f, &config, icon_ranges);
    io.Fonts->Build();
    ImGui::SFML::UpdateFontTexture();
}

void UI::push_font(unsigned size) {
    int s = static_cast<int>(size);
    if (auto verfuegbar = ImGui::GetIO().Fonts->Fonts.size(); s > verfuegbar) {
        std::cerr << "[Fehler] Ungueltige Schriftgroesse: " << size << ", max = " << verfuegbar << std::endl;
    }
    ImFont* font = ImGui::GetIO().Fonts->Fonts[static_cast<int>(s)];
    ImGui::PushFont(font);
}

void UI::push_icons() {
    ImFont* font = ImGui::GetIO().Fonts->Fonts.back();
    ImGui::PushFont(font);
}

void UI::tooltip(const char* text) {
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(text);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
