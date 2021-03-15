//
// Created by alex on 28.11.20.
//

#include "bcg_gui_mesh_statistics.h"
#include "mesh/bcg_mesh_statistics.h"
#include "viewer/bcg_viewer_state.h"

namespace bcg{

void gui_mesh_statistics(viewer_state *state){
    if(!state->scene.valid(state->picker.entity_id)) return;
    if(!state->scene.all_of<halfedge_mesh>(state->picker.entity_id)) return;
    if(!state->scene.all_of<mesh_stats>(state->picker.entity_id)){
        state->dispatcher.trigger<event::mesh::statistics>(state->picker.entity_id);
    }
    auto &stats = state->scene.get<mesh_stats>(state->picker.entity_id);
    ImGui::Text("vertices:");
    draw_label(&state->window, "min valence", std::to_string(stats.vertices.min_valence));
    draw_label(&state->window, "max valence", std::to_string(stats.vertices.max_valence));
    draw_label(&state->window, "avg valence", std::to_string(stats.vertices.avg_valence));
    ImGui::Separator();
    ImGui::Text("edges:");
    draw_label(&state->window, "min length", std::to_string(stats.edges.min_length));
    draw_label(&state->window, "max length", std::to_string(stats.edges.max_length));
    draw_label(&state->window, "avg length", std::to_string(stats.edges.avg_length));
    ImGui::Separator();
    ImGui::Text("faces:");
    draw_label(&state->window, "min area", std::to_string(stats.faces.min_area));
    draw_label(&state->window, "max area", std::to_string(stats.faces.max_area));
    draw_label(&state->window, "avg area", std::to_string(stats.faces.avg_area));
    ImGui::Separator();
    if(ImGui::Button("Recompute")){
        state->dispatcher.trigger<event::mesh::statistics>(state->picker.entity_id);
    }
}

}