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
}
