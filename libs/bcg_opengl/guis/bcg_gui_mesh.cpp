//
// Created by alex on 16.11.20.
//

#include "bcg_gui_mesh.h"
#include "bcg_viewer_state.h"
#include "bcg_gui_property_container_selector.h"

namespace bcg{

void gui_mesh(viewer_state *state, halfedge_mesh *mesh){
    if (!mesh) return;
    if (ImGui::CollapsingHeader("mesh")) {
        gui_property_container_selector(state, &mesh->vertices, state->picker.vertex_id.idx);
        gui_property_container_selector(state, &mesh->halfedges, state->picker.halfedge_id.idx);
        gui_property_container_selector(state, &mesh->edges, state->picker.edge_id.idx);
        gui_property_container_selector(state, &mesh->faces, state->picker.face_id.idx);
        ImGui::Separator();
    }
}

}