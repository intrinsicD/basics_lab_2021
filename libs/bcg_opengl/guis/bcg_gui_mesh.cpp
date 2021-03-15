//
// Created by alex on 16.11.20.
//

#include "bcg_gui_mesh.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_gui_property_container_selector.h"
#include "bcg_gui_property_editfield.h"

namespace bcg{

void gui_mesh(viewer_state *state, halfedge_mesh *mesh){
    if (!mesh) return;
    static bool show_vertices_edit = false;
    static bool show_halfedges_edit = false;
    static bool show_edges_edit = false;
    static bool show_faces_edit = false;
    if (ImGui::CollapsingHeader("mesh properties")) {
        gui_property_container_selector(state, &mesh->vertices, state->picker.vertex_id.idx);
        gui_property_container_selector(state, &mesh->halfedges, state->picker.halfedge_id.idx);
        gui_property_container_selector(state, &mesh->edges, state->picker.edge_id.idx);
        gui_property_container_selector(state, &mesh->faces, state->picker.face_id.idx);
        if(ImGui::Button("edit vertices")){
            show_vertices_edit = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("edit halfedges")){
            show_halfedges_edit = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("edit edges")){
            show_edges_edit = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("edit faces")){
            show_faces_edit = true;
        }
        ImGui::Separator();
    }
    if(show_vertices_edit) edit_field(state, &mesh->vertices, "edit vertices", show_vertices_edit);
    if(show_halfedges_edit) edit_field(state, &mesh->halfedges, "edit halfedges", show_vertices_edit);
    if(show_edges_edit) edit_field(state, &mesh->edges, "edit edges", show_vertices_edit);
    if(show_faces_edit) edit_field(state, &mesh->faces, "edit faces", show_vertices_edit);
}

}