//
// Created by alex on 16.11.20.
//

#include <sstream>
#include "bcg_gui_transform.h"
#include "bcg_viewer_state.h"

namespace bcg{

void gui_transform(viewer_state *state, Transform *model){
    if (!model) return;
    if (ImGui::CollapsingHeader("transform")) {
        std::stringstream ss;
        ss << model->translation().transpose();
        draw_label(&state->window, "position", ss.str());
        ss.str("");
        ss << model->linear();
        draw_label(&state->window, "rotation\n\n\n", ss.str());
        ss.str("");
        ss << model->matrix();
        draw_label(&state->window, "model_matrix\n\n\n\n", ss.str());
    }
}

}