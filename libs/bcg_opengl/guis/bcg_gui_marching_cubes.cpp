//
// Created by alex on 25.11.20.
//

#include <renderers/mesh_renderer/bcg_events_mesh_renderer.h>
#include "geometry/marching_cubes/bcg_marching_cubes.h"
#include "bcg_gui_marching_cubes.h"
#include "viewer/bcg_viewer_state.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"

namespace bcg {

void gui_marching_cubes(viewer_state *state) {
    static VectorS<3> min = -2 * VectorS<3>::Ones();
    static VectorS<3> max = 2 * VectorS<3>::Ones();
    static VectorS<3> dims = {30, 30, 30};
    static bcg_scalar_t isovalue = 0;
    draw_input(&state->window, "isovalue", isovalue);
    draw_input_vec3(&state->window, "min", min);
    draw_input_vec3(&state->window, "max", max);
    draw_input_vec3(&state->window, "dims", dims);
    static marching_cubes mc;
    mc.implicit_function = marching_cubes::hearts_function;
    if (ImGui::Button("convert to mesh")) {
        auto mesh = mc.reconstruct(isovalue, min, max, dims.cast<bcg_index_t>());
        auto id = state->scene.create();
        state->scene.emplace<halfedge_mesh>(id, mesh);
        state->dispatcher.trigger<event::mesh::setup>(id, "marching cubes");
    }

    if (ImGui::Button("compute vertex normals")) {
        if (state->scene.valid(state->picker.entity_id)) {
            auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
            mc.compute_vertex_normals(mesh);
            auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
            state->dispatcher.trigger<event::mesh_renderer::set_normal_attribute>(state->picker.entity_id,
                                                                                  material.attributes[1]);
        }
    }
}

}