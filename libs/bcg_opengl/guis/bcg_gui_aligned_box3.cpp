//
// Created by alex on 16.11.20.
//

#include <sstream>
#include "bcg_gui_aligned_box3.h"
#include "bcg_viewer_state.h"

namespace bcg{

void gui_aligned_box3(viewer_state *state, aligned_box3 *aabb){
    if (!aabb) return;
    if (ImGui::CollapsingHeader("aligned_box")) {
        std::stringstream ss;
        ss << aabb->min.transpose();
        draw_label(&state->window, "min", ss.str());
        ss.str("");
        ss << aabb->max.transpose();
        draw_label(&state->window, "max", ss.str());
        ss.str("");
        ss << aabb->center().transpose();
        draw_label(&state->window, "center", ss.str());
        ss.str("");
        ss << aabb->halfextent().transpose();
        draw_label(&state->window, "halfextent", ss.str());
        ss.str("");
        ImGui::Separator();
    }
}

}