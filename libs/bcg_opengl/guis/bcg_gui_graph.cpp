//
// Created by alex on 16.11.20.
//

#include "bcg_gui_graph.h"
#include "bcg_viewer_state.h"
#include "bcg_gui_property_container_selector.h"

namespace bcg{

void gui_graph(viewer_state *state, halfedge_graph *graph){
    if (!graph) return;
    if (ImGui::CollapsingHeader("graph")) {
        gui_property_container_selector(state, &graph->vertices, state->picker.vertex_id.idx);
        gui_property_container_selector(state, &graph->halfedges, state->picker.halfedge_id.idx);
        gui_property_container_selector(state, &graph->edges, state->picker.edge_id.idx);
    }
}

}