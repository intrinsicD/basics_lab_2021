//
// Created by alex on 16.11.20.
//

#include "bcg_gui_point_cloud.h"
#include "bcg_gui_property_edit.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_gui_property_container_selector.h"

namespace bcg {

void gui_point_cloud(viewer_state *state) {
    gui_point_cloud(state, state->picker.entity_id);
}

void gui_point_cloud(viewer_state *state, entt::entity id) {
    if (!state->scene.valid(id)) return;
    if (!state->scene.has<point_cloud>(id)) return;
    gui_point_cloud(state, state->scene.get<point_cloud>(id));
}


void gui_point_cloud(viewer_state *state, point_cloud &pc) {
    gui_property_container_selector(state, &pc.vertices, state->picker.vertex_id.idx);
    ImGui::Separator();
    { gui_property_edit(state, &pc.vertices, "pc edit vertices"); }
}

}
