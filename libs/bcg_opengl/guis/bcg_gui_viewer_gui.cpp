//
// Created by alex on 17.11.20.
//

#include "bcg_gui_viewer_gui.h"
#include "viewer/bcg_viewer_state.h"

namespace bcg{

void gui_viewer_gui(viewer_state *state){
    if (ImGui::CollapsingHeader("Gui")) {
        ImGui::Checkbox("widgets_active", &state->gui.widgets_active);
        ImGui::Checkbox("show_menu", &state->gui.show_menu);
        if(ImGui::Checkbox("hide_all", &state->gui.hide_all)){
            state->gui.captured_keyboard = false;
            state->gui.captured_mouse = false;
            state->gui.widgets_active = false;
        }
        draw_label(&state->window, "menu_height", std::to_string(state->gui.menu_height));
        ImGui::Separator();
    }
}

}