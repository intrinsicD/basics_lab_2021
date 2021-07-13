//
// Created by alex on 13.07.21.
//

#include "bcg_gui_mesh_geodesic_distances_heat_method.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_gui_mesh_laplacian.h"
#include "math/laplacian/bcg_laplacian_heat_method.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"

namespace bcg {

void gui_mesh_geodesic_heat_method(viewer_state *state) {
    static float m = 1;
    static float t = 1;
    ImGui::InputFloat("m", &m);
    ImGui::InputFloat("t", &t);
    auto id = state->picker.entity_id;
    if (ImGui::Button("Compute t")) {
        if (state->scene.valid(id) && state->scene.has<halfedge_mesh>(id)) {
            auto &mesh = state->scene.get<halfedge_mesh>(id);
            t = compute_t_squared(mesh, m);
        }
    }
    gui_mesh_laplacian(state);
    ImGui::Separator();
    static bool vertex_selection_mode = state->picker.mode == viewer_picker::Mode::vertices;
    if (ImGui::Checkbox("vertex_selection_mode", &vertex_selection_mode)) {
        if (vertex_selection_mode) {
            state->dispatcher.trigger<event::picker::enable::vertex>();
        } else {
            state->dispatcher.trigger<event::picker::disable>();
        }
        vertex_selection_mode = state->picker.mode == viewer_picker::Mode::vertices;
    }
    if (ImGui::Button("Compute Geodesics")) {
        if (state->scene.has<mesh_laplacian>(state->picker.entity_id)) {
            if (state->scene.valid(id) && state->scene.has<halfedge_mesh>(id)) {
                auto &mesh = state->scene.get<halfedge_mesh>(id);
                auto &laplacian = state->scene.get<mesh_laplacian>(state->picker.entity_id);
                heat_method(mesh, laplacian, t, state->config.parallel_grain_size);
                auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
                auto &color = material.attributes[2];
                color.property_name = "v_geodesic_distance_heat_method";
                state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
            }
        }
    }


}


}