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
    if(ImGui::Button("Hearts Function")){
        mc.implicit_function = marching_cubes::hearts_function;
        min = -2 * VectorS<3>::Ones();
        max = 2 * VectorS<3>::Ones();
    }
    if(ImGui::Button("Convert Point Cloud Ellipses")){
        if(state->scene.valid(state->picker.entity_id)){
            auto *vertices = state->get_vertices(state->picker.entity_id);
            if(vertices){
                auto &aabb = state->scene.get<aligned_box3>(state->picker.entity_id);
                auto positions = vertices->get<VectorS<3>, 3>("v_position");
                mc.implicit_function = mc.convert_point_cloud_ellipse(positions);
                min = aabb.min;
                max = aabb.max;
            }
        }
    }
    if(ImGui::Button("Convert Point Cloud Cassini")){
        if(state->scene.valid(state->picker.entity_id)){
            auto *vertices = state->get_vertices(state->picker.entity_id);
            if(vertices){
                auto &aabb = state->scene.get<aligned_box3>(state->picker.entity_id);
                auto positions = vertices->get<VectorS<3>, 3>("v_position");
                mc.implicit_function = mc.convert_point_cloud_cassini(positions);
                min = aabb.min;
                max = aabb.max;
            }
        }
    }
    if (ImGui::Button("convert to mesh")) {
        auto mesh = mc.reconstruct(isovalue, min, max, dims.cast<bcg_index_t>());
        if(!mesh.empty()){
            auto id = state->scene.create();
            state->scene().emplace<halfedge_mesh>(id, mesh);
            state->dispatcher.trigger<event::mesh::setup>(id, "marching cubes");
        }
    }
    ImGui::LabelText("avg_sdf", "%f", mc.avg_sdf);
    ImGui::LabelText("min_sdf", "%f", mc.min_sdf);
    ImGui::LabelText("max_sdf", "%f", mc.max_sdf);

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