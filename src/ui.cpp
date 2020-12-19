#include "ui.hpp"

#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>

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
    // 12 14 16 18 20 22 24
    for (unsigned i = 12; i <= 24; i += 2) {
        io.Fonts->AddFontFromFileTTF(
                "data/RursusCompactMono.ttf", static_cast<float>(i), nullptr, io.Fonts->GetGlyphRangesCyrillic()
         );
    }

    // Icons laden
    ImFontConfig config;
    config.GlyphMinAdvanceX = 16.0f; // Monospaced
    static const ImWchar icon_ranges[] = {0xF000, 0xF2E0, 0};
    io.Fonts->AddFontFromFileTTF("data/icons/fontawesome-webfont.ttf", 16.f, &config, icon_ranges);
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
    push_icons();
    ImGui::SameLine();
    ImGui::TextDisabled("\uF059"); // (?)
    ImGui::PopFont();
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(text);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
