//
// Created by alex on 27.11.20.
//

#include "bcg_gui_mesh_remeshing.h"
#include "bcg_viewer_state.h"
#include "bcg_gui_mesh_statistics.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"

namespace bcg {


void gui_mesh_remeshing(viewer_state *state) {
    if(ImGui::CollapsingHeader("Statistics")){
        gui_mesh_statistics(state);
    }
    static int e = 0;
    ImGui::RadioButton("uniform", &e, 0);
    ImGui::SameLine();
    ImGui::RadioButton("adaptive", &e, 1);
    static int iterations = 1;
    static bool use_projection = true;
    static bcg_scalar_t edge_length = 0;
    static bcg_scalar_t min_edge_length = 0;
    static bcg_scalar_t max_edge_length = 0;
    static bcg_scalar_t approx_error = 0;
    if (e == 0) {
        ImGui::InputFloat("edge_length", &edge_length);
    } else if (e == 1) {
        ImGui::InputFloat("min_edge_length", &min_edge_length);
        ImGui::InputFloat("max_edge_length", &max_edge_length);
        ImGui::InputFloat("approx_error", &approx_error);
    }
    ImGui::InputInt("iterations", &iterations);
    ImGui::Checkbox("use projection", &use_projection);
    if (ImGui::Button("Compute")) {
        if (e == 0) {
            state->dispatcher.trigger<event::mesh::remeshing::uniform>(state->picker.entity_id,
                                                                       edge_length,
                                                                       (unsigned int) iterations,
                                                                       use_projection);
        }
        if (e == 1) {
            state->dispatcher.trigger<event::mesh::remeshing::adaptive>(state->picker.entity_id, min_edge_length,
                                                                        max_edge_length, approx_error,
                                                                        (unsigned int) iterations, use_projection);
        }

        auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(state->picker.entity_id, material.attributes);
        auto edge_attributes = {attribute{"edges", "edges", "edges", 0, true}};
        state->dispatcher.trigger<event::gpu::update_edge_attributes>(state->picker.entity_id, edge_attributes);
        auto face_attributes = {attribute{"triangles", "triangles", "triangles", 0, true}};
        state->dispatcher.trigger<event::gpu::update_face_attributes>(state->picker.entity_id, face_attributes);
    }
}

}
