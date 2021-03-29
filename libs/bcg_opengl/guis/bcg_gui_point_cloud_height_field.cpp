//
// Created by alex on 29.03.21.
//

#include "bcg_gui_point_cloud_height_field.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_library/geometry/point_cloud/bcg_point_cloud_height_field.h"
#include "bcg_gui_property_selector.h"
#include "bcg_opengl/renderers/mesh_renderer/bcg_material_mesh.h"
#include "bcg_opengl/renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "bcg_opengl/renderers/points_renderer/bcg_material_points.h"
#include "bcg_opengl/renderers/points_renderer/bcg_events_points_renderer.h"
namespace bcg{

void gui_point_cloud_height_field(viewer_state *state){
    static std::string current_property_name;
    auto *vertices = state->get_vertices(state->picker.entity_id);
    if(state->scene.valid(state->picker.entity_id) && vertices){
        gui_property_selector(state, vertices, {1}, "properties", current_property_name);
    }
    static int dim = 1;
    static float alpha = 0.4;
    static bool invert = false;
    ImGui::InputInt("dimension", &dim);
    if(!current_property_name.empty()){
        ImGui::InputFloat("alpha", &alpha);
        ImGui::Checkbox("invert", &invert);
    }
    if(ImGui::Button("Compute Height Field")){
        if(vertices){
            static property<bcg_scalar_t, 1> weighting;
            if(!current_property_name.empty()){
                weighting = vertices->get<bcg_scalar_t, 1>(current_property_name);
            }
            point_cloud_height_field(vertices, weighting, bcg_scalar_t(alpha), dim, invert);
            if(state->scene.all_of<halfedge_mesh>(state->picker.entity_id)){
                auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
                auto &color = material.attributes[2];
                color.property_name = "v_height_field";
                state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
            }else{
                auto &material = state->scene.get<material_points>(state->picker.entity_id);
                auto &color = material.attributes[1];
                color.property_name = "v_height_field";
                state->dispatcher.trigger<event::points_renderer::set_color_attribute>(state->picker.entity_id, color);
            }
        }
    }
}

}