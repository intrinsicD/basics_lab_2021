//
// Created by alex on 16.11.20.
//

#include "bcg_gui_mesh.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_gui_property_container_selector.h"
#include "bcg_gui_property_edit.h"

namespace bcg {

void gui_mesh(viewer_state *state){
    gui_mesh(state, state->picker.entity_id);
}

void gui_mesh(viewer_state *state, entt::entity id){
    if(!state->scene.valid(id)) return;
    if(!state->scene.has<halfedge_mesh>(id)) return;
    gui_mesh(state, state->scene.get<halfedge_mesh>(id));
}

void gui_mesh(viewer_state *state, halfedge_mesh &mesh){
    gui_property_container_selector(state, &mesh.vertices, state->picker.vertex_id.idx);
    gui_property_container_selector(state, &mesh.halfedges, state->picker.halfedge_id.idx);
    gui_property_container_selector(state, &mesh.edges, state->picker.edge_id.idx);
    gui_property_container_selector(state, &mesh.faces, state->picker.face_id.idx);
    { gui_property_edit(state, &mesh.vertices, "edit mesh vertices"); }
    { gui_property_edit(state, &mesh.halfedges, "edit mesh halfedges"); }
    { gui_property_edit(state, &mesh.edges, "edit mesh edges"); }
    { gui_property_edit(state, &mesh.faces, "edit mesh faces"); }
}

}