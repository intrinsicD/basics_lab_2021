//
// Created by alex on 27.11.20.
//

#include "bcg_gui_mesh_simplification.h"
#include "bcg_viewer_state.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"

namespace bcg {

void gui_mesh_simplification(viewer_state *state) {
    static int n_vertices;
    static bcg_scalar_t aspect_ratio = 0.0;
    static bcg_scalar_t edge_length = 0.0;
    static int max_valence = 0;
    static bcg_scalar_t normal_deviation = 0.0;
    static bcg_scalar_t hausdorff_error = 0.0;
    ImGui::InputInt("num vertices", &n_vertices);
    draw_input(&state->window, "aspect ratio", aspect_ratio);
    draw_input(&state->window, "edge length", edge_length);
    ImGui::InputInt("max valence", &max_valence);
    draw_input(&state->window, "normal deviation", normal_deviation);
    draw_input(&state->window, "hausdorff error", hausdorff_error);
    if (ImGui::Button("Compute")) {
        state->dispatcher.trigger<event::mesh::simplification>(state->picker.entity_id, (unsigned int) n_vertices,
                                                               aspect_ratio,
                                                               edge_length, (unsigned int) max_valence,
                                                               normal_deviation,
                                                               hausdorff_error);

        auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(state->picker.entity_id, material.attributes);
        auto edge_attributes = {attribute{"edges", "edges", "edges", 0, true}};
        state->dispatcher.trigger<event::gpu::update_edge_attributes>(state->picker.entity_id, edge_attributes);
        auto face_attributes = {attribute{"triangles", "triangles", "triangles", 0, true}};
        state->dispatcher.trigger<event::gpu::update_face_attributes>(state->picker.entity_id, face_attributes);
    }
}

}