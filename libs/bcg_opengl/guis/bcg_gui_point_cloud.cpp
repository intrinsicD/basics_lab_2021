//
// Created by alex on 16.11.20.
//

#include "bcg_gui_point_cloud.h"
#include "bcg_viewer_state.h"
#include "bcg_gui_property_container_selector.h"

namespace bcg{

void gui_point_cloud(viewer_state *state, point_cloud *pc){
    if (!pc) return;
    if (ImGui::CollapsingHeader("point_cloud")) {
        gui_property_container_selector(state, &pc->vertices, state->picker.vertex_id.idx);
    }
}

}
