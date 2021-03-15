//
// Created by alex on 17.11.20.
//

#include "bcg_gui_viewer_colors.h"
#include "viewer/bcg_viewer_state.h"

namespace bcg{

void gui_viewer_colors(viewer_state *state){
    if (ImGui::CollapsingHeader("Colors")) {
        draw_coloredit(&state->window, "background", state->colors.background);
        draw_coloredit(&state->window, "overlay", state->colors.overlay);
        draw_coloredit(&state->window, "entity_selection", state->colors.entity_selection);
        draw_coloredit(&state->window, "vertex_selection", state->colors.vertex_selection);
        draw_coloredit(&state->window, "edge_selection", state->colors.edge_selection);
        draw_coloredit(&state->window, "face_selection", state->colors.face_selection);
        draw_coloredit(&state->window, "boundary", state->colors.boundary);
        draw_coloredit(&state->window, "wireframe", state->colors.wireframe);
        ImGui::Separator();
    }
}

}