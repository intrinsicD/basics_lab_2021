//
// Created by alex on 16.11.20.
//

#include "bcg_gui_graph.h"
#include "bcg_viewer_state.h"
#include "bcg_gui_property_container_selector.h"
#include "bcg_gui_property_editfield.h"

namespace bcg{

void gui_graph(viewer_state *state, halfedge_graph *graph){
    if (!graph) return;
    static bool show_vertices_edit = false;
    static bool show_halfedges_edit = false;
    static bool show_edges_edit = false;
    if (ImGui::CollapsingHeader("graph properties")) {
        gui_property_container_selector(state, &graph->vertices, state->picker.vertex_id.idx);
        gui_property_container_selector(state, &graph->halfedges, state->picker.halfedge_id.idx);
        gui_property_container_selector(state, &graph->edges, state->picker.edge_id.idx);
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
        ImGui::Separator();
    }
    if(show_vertices_edit) edit_field(state, &graph->vertices, "edit vertices", show_vertices_edit);
    if(show_halfedges_edit) edit_field(state, &graph->halfedges, "edit halfedges", show_vertices_edit);
    if(show_edges_edit) edit_field(state, &graph->edges, "edit faces", show_vertices_edit);
}

}