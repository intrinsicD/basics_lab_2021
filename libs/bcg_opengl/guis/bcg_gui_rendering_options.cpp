//
// Created by alex on 16.11.20.
//

#include "bcg_gui_rendering_options.h"
#include "bcg_viewer_state.h"
#include "bcg_gui_materials.h"
#include "renderers/points_renderer/bcg_events_points_renderer.h"
#include "renderers/graph_renderer/bcg_events_graph_renderer.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "renderers/vectorfield_renderer/bcg_events_vectorfield_renderer.h"
#include "renderers/curve_renderer/bcg_events_curve_renderer.h"

namespace bcg{

void gui_rendering_options(viewer_state *state, entt::entity id){
    static bool show_points = state->scene.has<point_cloud>(id);
    static bool show_edges = state->scene.has<halfedge_graph>(id);
    static bool show_mesh = state->scene.has<halfedge_mesh>(id);
    static bool show_vectors = false;
    static bool show_curves = false;
    if (ImGui::CollapsingHeader("rendering")) {
        if (ImGui::Checkbox("show points", &show_points)) {
            if (show_points) {
                state->scene.emplace_or_replace<event::points_renderer::enqueue>(id);
            } else {
                state->scene.remove_if_exists<event::points_renderer::enqueue>(id);
            }
        }
        if (ImGui::Checkbox("show edges", &show_edges)) {
            if (show_edges && state->get_edges(id) != nullptr) {
                state->scene.emplace_or_replace<event::graph_renderer::enqueue>(id);
            } else {
                state->scene.remove_if_exists<event::graph_renderer::enqueue>(id);
            }
        }
        if (ImGui::Checkbox("show mesh", &show_mesh)) {
            if (show_mesh && state->get_faces(id) != nullptr) {
                state->scene.emplace_or_replace<event::mesh_renderer::enqueue>(id);
            } else {
                state->scene.remove_if_exists<event::mesh_renderer::enqueue>(id);
            }
        }
        if (ImGui::Checkbox("show vectors", &show_vectors)) {
            if (show_vectors) {
                state->scene.emplace_or_replace<event::vectorfield_renderer::enqueue>(id);
            } else {
                state->scene.remove_if_exists<event::vectorfield_renderer::enqueue>(id);
            }
        }
        if (ImGui::Checkbox("show curves", &show_curves)) {
            if (show_curves) {
                state->scene.emplace_or_replace<event::curve_renderer::enqueue>(id);
            } else {
                state->scene.remove_if_exists<event::curve_renderer::enqueue>(id);
            }
        }
    }

    gui_materials(state, id, show_points, show_edges, show_mesh, show_vectors, show_curves);
}

}