//
// Created by alex on 11.05.21.
//

#include "bcg_gui_graph_spring_embedding.h"
#include "viewer/bcg_viewer_state.h"
#include "graph/bcg_graph_spring_embedding.h"

namespace bcg {

void gui_graph_spring_embedding(viewer_state *state) {
    if (ImGui::Button("Random 2D embedding")) {
        auto id = state->picker.entity_id;
        if (state->scene.valid(id) && state->get_edges(id) != nullptr) {
            if (state->scene.all_of<halfedge_mesh>(id)) {
                auto &mesh = state->scene.get<halfedge_mesh>(id);
                graph_random_embedding_2D(mesh);
            } else if (state->scene.all_of<halfedge_graph>(id)) {
                auto &graph = state->scene.get<halfedge_graph>(id);
                graph_random_embedding_2D(graph);
            }
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Random 3D embedding")) {
        auto id = state->picker.entity_id;
        if (state->scene.valid(id) && state->get_edges(id) != nullptr) {
            if (state->scene.all_of<halfedge_mesh>(id)) {
                auto &mesh = state->scene.get<halfedge_mesh>(id);
                graph_random_embedding_3D(mesh);
            } else if (state->scene.all_of<halfedge_graph>(id)) {
                auto &graph = state->scene.get<halfedge_graph>(id);
                graph_random_embedding_3D(graph);
            }
        }
    }
    ImGui::Separator();
    static bool run_each_frame = false;
    static graph_spring_embedding embedding;
    ImGui::InputInt("num_steps", &embedding.num_steps);
    ImGui::InputFloat("force_eps", &embedding.force_eps);
    ImGui::InputFloat("force_coefficient", &embedding.force_coefficient);
    ImGui::InputFloat("repulsion_coefficient", &embedding.repulsion_coefficient);
    ImGui::InputFloat("spring_coefficient", &embedding.spring_coefficient);
    ImGui::InputFloat("target_edge_length", &embedding.target_edge_length);
    ImGui::Checkbox("annealing", &embedding.annealing);
    ImGui::Checkbox("run_each_frame", &run_each_frame);
    ImGui::Separator();
    if (ImGui::Button("Iterate Spring Embedding") || run_each_frame) {
        auto id = state->picker.entity_id;
        if (state->scene.valid(id) && state->get_edges(id) != nullptr) {
            if (state->scene.all_of<halfedge_mesh>(id)) {
                auto &mesh = state->scene.get<halfedge_mesh>(id);
                embedding.iterate(mesh);
            } else if (state->scene.all_of<halfedge_graph>(id)) {
                auto &graph = state->scene.get<halfedge_graph>(id);
                embedding.iterate(graph);
            }
        }
    }
}

}