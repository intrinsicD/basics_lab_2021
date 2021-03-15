//
// Created by alex on 17.11.20.
//

#include "bcg_gui_viewer_time.h"
#include "viewer/bcg_viewer_state.h"

namespace bcg{

void gui_viewer_time(viewer_state *state){
    if (ImGui::CollapsingHeader("Time")) {
        draw_label(&state->window, "clock_now", std::to_string(state->time.clock_now));
        draw_label(&state->window, "clock_last", std::to_string(state->time.clock_last));
        draw_label(&state->window, "time_now", std::to_string(state->time.time_now));
        draw_label(&state->window, "time_delta", std::to_string(state->time.time_delta));
        ImGui::Separator();
    }
}

}