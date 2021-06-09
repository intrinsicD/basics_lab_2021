//
// Created by alex on 17.11.20.
//

#include "bcg_gui_viewer_colors.h"
#include "viewer/bcg_viewer_state.h"

namespace bcg{

void gui_viewer_colors(viewer_state *state){
    if (ImGui::CollapsingHeader("Colors")) {
        draw_coloredit(&state->window, "background", state->colors.background);
        ImGui::Separator();
    }
}

}