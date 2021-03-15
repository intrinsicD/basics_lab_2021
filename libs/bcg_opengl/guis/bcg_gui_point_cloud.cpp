//
// Created by alex on 16.11.20.
//

#include "bcg_gui_point_cloud.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_gui_property_container_selector.h"
#include "bcg_gui_property_editfield.h"

namespace bcg{

void gui_point_cloud(viewer_state *state, point_cloud *pc){
    if (!pc) return;
    static bool show_vertices_edit = false;
    if (ImGui::CollapsingHeader("point_cloud properties")) {
        gui_property_container_selector(state, &pc->vertices, state->picker.vertex_id.idx);
        if(ImGui::Button("edit vertices")){
            show_vertices_edit = true;
        }
        ImGui::Separator();
    }
    if(show_vertices_edit) edit_field(state, &pc->vertices, "edit vertices",show_vertices_edit);
}

}
