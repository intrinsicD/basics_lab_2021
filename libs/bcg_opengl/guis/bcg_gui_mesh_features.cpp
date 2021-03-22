//
// Created by alex on 19.11.20.
//

#include "bcg_gui_mesh_features.h"
#include "viewer/bcg_viewer_state.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "renderers/graph_renderer/bcg_material_graph.h"
#include "renderers/graph_renderer/bcg_events_graph_renderer.h"

namespace bcg {

struct viewer_state;

void gui_mesh_features(viewer_state *state) {
    static float threshold_angle = 45.0f;
    static bool boundary = true;
    static bool angle = true;
    ImGui::InputFloat("threshold angle", &threshold_angle, 0.01f, 0.1f);
    ImGui::Checkbox("boundary", &boundary);
    ImGui::Checkbox("dihedral angle", &angle);
    if (ImGui::Button("compute features")) {
        state->dispatcher.trigger<event::mesh::features>(state->picker.entity_id, boundary, angle, threshold_angle);
        {
            auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
            auto &color = material.attributes[2];
            color.property_name = "v_feature";
            state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
        }
        {
            auto &material = state->scene.get_or_emplace<material_graph>(state->picker.entity_id);
            auto &color = material.attributes[1];
            color.property_name = "e_feature";
            state->dispatcher.trigger<event::graph_renderer::set_color_texture>(state->picker.entity_id, color);
        }
    }
    if (ImGui::Button("clear features")) {
        state->dispatcher.trigger<event::mesh::features_clear>(state->picker.entity_id);
        {
            auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
            material.use_uniform_color = true;
        }
        {
            state->scene.remove_if_exists<event::graph_renderer::enqueue>(state->picker.entity_id);
        }
    }
}

}
