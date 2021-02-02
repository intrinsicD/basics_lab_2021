//
// Created by alex on 27.11.20.
//

#include "bcg_gui_mesh_curvature_taubin.h"
#include "bcg_viewer_state.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"

namespace bcg {

void gui_mesh_curvature_taubin(viewer_state *state) {
    static int post_smoothing_steps = 3;
    bool two_ring_neighborhood = true;
    ImGui::InputInt("post smoothing steps", &post_smoothing_steps);
    ImGui::Checkbox("two ring neighborhood", &two_ring_neighborhood);
    if (ImGui::Button("Compute Curvature")) {
        state->dispatcher.trigger<event::mesh::curvature::taubin>(state->picker.entity_id, post_smoothing_steps, two_ring_neighborhood);
        auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
        auto &color = material.attributes[2];
        color.property_name = "v_mesh_curv_mean";
        state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
    }
}

}