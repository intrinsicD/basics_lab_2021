//
// Created by alex on 19.11.20.
//

#include "bcg_gui_mesh_vertex_convex_concave.h"
#include "bcg_gui_property_classify_max.h"
#include "bcg_property_map_eigen.h"
#include "mesh/bcg_mesh_vertex_convex_concave.h"
#include "graph/bcg_graph_edge_from_vertex_function.h"
#include "viewer/bcg_viewer_state.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"

namespace bcg {

void gui_mesh_vertex_convex_concave(viewer_state *state) {
    static int post_smoothing_steps = 3;
    static bool two_ring_neighborhood = true;
    ImGui::InputInt("post smoothing steps", &post_smoothing_steps);
    ImGui::Checkbox("two ring neighborhood", &two_ring_neighborhood);
    if (ImGui::Button("compute vertex convex concave")) {
        state->dispatcher.trigger<event::mesh::vertex_convex_concave>(state->picker.entity_id, post_smoothing_steps,
                                                                      two_ring_neighborhood);
        auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
        auto &color = material.attributes[2];
        color.property_name = "v_convex";
        state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
    }

    static float value_small = 0.1;
    static float value_large = 1000;
    if (state->scene.valid(state->picker.entity_id)) {
        if (state->scene.has<halfedge_mesh>(state->picker.entity_id)) {
            auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
            if (mesh.vertices.has("v_concave")) {
                auto result = gui_property_classify_max(state);
                ImGui::InputFloat("value_small", &value_small);
                ImGui::InputFloat("value_large", &value_large);
                if (ImGui::Button("Compute Edge Scaling")) {
                    auto v_concave_theshold = mesh.vertices.get<bcg_scalar_t, 1>(result.current_property_name);
                    if (v_concave_theshold) {
                        edge_from_vertex_boolean_function_or(mesh, "e_laplacian_scaling", v_concave_theshold,
                                                             value_small,
                                                             value_large, state->config.parallel_grain_size);
                    }
                }
            }
        }
    }
}

}