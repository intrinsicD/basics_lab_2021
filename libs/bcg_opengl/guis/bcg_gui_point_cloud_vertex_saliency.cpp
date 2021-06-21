//
// Created by alex on 29.03.21.
//

#include "bcg_gui_point_cloud_vertex_saliency.h"
#include "bcg_gui_radius_from_aabb.h"
#include "bcg_gui_property_selector.h"
#include "viewer/bcg_viewer_state.h"
#include "point_cloud/bcg_point_cloud_vertex_saliency.h"
#include "bcg_opengl/renderers/mesh_renderer/bcg_material_mesh.h"
#include "bcg_opengl/renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "bcg_opengl/renderers/points_renderer/bcg_material_points.h"
#include "bcg_opengl/renderers/points_renderer/bcg_events_points_renderer.h"

namespace bcg{

void gui_point_cloud_vertex_saliency(viewer_state *state){
    static std::string current_property_name;
    static float radius;
    if(state->scene.valid(state->picker.entity_id)){
        auto *vertices = state->get_vertices(state->picker.entity_id);
        if(vertices){
            gui_property_selector(state, vertices, {1}, "property", current_property_name);

            radius = gui_radius_from_aabb(state);
            if(ImGui::Button("Compute Saliency")){
                auto property = vertices->get<bcg_scalar_t, 1>(current_property_name);
                state->dispatcher.trigger<event::spatial_index::setup_kdtree>(state->picker.entity_id);
                auto &index = state->scene.get<kdtree_property<bcg_scalar_t>>(state->picker.entity_id);
                point_cloud_vertex_saliency(vertices, property, index, radius, state->config.parallel_grain_size);
                std::string property_name = "v_saliency_diff";
                if(state->scene.has<halfedge_mesh>(state->picker.entity_id)){
                    auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
                    auto &color = material.attributes[2];
                    color.property_name = property_name;
                    state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
                }else{
                    auto &material = state->scene.get<material_points>(state->picker.entity_id);
                    auto &color = material.attributes[1];
                    color.property_name = property_name;
                    state->dispatcher.trigger<event::points_renderer::set_color_attribute>(state->picker.entity_id, color);
                }
            }
        }
    }
}

}