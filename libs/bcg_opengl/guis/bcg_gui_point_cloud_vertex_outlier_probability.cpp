//
// Created by alex on 03.12.20.
//

#include "bcg_gui_point_cloud_vertex_outlier_probability.h"
#include "bcg_gui_kdtree_selector.h"
#include "bcg_gui_material_points.h"
#include "renderers/points_renderer/bcg_events_points_renderer.h"
#include "renderers/points_renderer/bcg_material_points.h"
#include "bcg_property_map_eigen.h"
#include "viewer/bcg_viewer_state.h"

namespace bcg{

void gui_point_cloud_vertex_outlier_probability(viewer_state *state){
    ImGui::PushID("pc_outliers");
    auto parameters = gui_kd_tree_selector(state);
    static float threshold = scalar_eps;
    if(ImGui::Button("Compute")){
        if(parameters.type == kdtree_parameters::Type::knn){
            state->dispatcher.trigger<event::point_cloud::vertex::outliers::probability_knn>(state->picker.entity_id, parameters.num_closest);
        }else if(parameters.type == kdtree_parameters::Type::radius){
            state->dispatcher.trigger<event::point_cloud::vertex::outliers::probability_radius>(state->picker.entity_id, parameters.radius);
        }

        auto *material = state->scene.try_get<material_points>(state->picker.entity_id);
        if(material){
            auto &color = material->attributes[1];
            color.property_name = "v_outlier_probability";
            state->dispatcher.trigger<event::points_renderer::set_color_attribute>(state->picker.entity_id, color);
            threshold = MapConst<bcg_scalar_t>(state->get_vertices(state->picker.entity_id)->get<bcg_scalar_t, 1>("v_outlier_probability")).mean();
        }
    }


    ImGui::InputFloat("threshold", &threshold);
    if(ImGui::Button("Preview Removal")){
        state->dispatcher.trigger<event::point_cloud::vertex::outliers::remove>(state->picker.entity_id, threshold);
        auto *material = state->scene.try_get<material_points>(state->picker.entity_id);
        if(material) {
            auto &color = material->attributes[1];
            color.property_name = "v_deleted";
            state->dispatcher.trigger<event::points_renderer::set_color_attribute>(state->picker.entity_id, color);
        }
    }
    if(ImGui::Button("Remove")){
        auto *pc = state->scene.try_get<point_cloud>(state->picker.entity_id);
        if(pc) {
            pc->garbage_collection();
            state->dispatcher.trigger<event::point_cloud::vertex::outliers::remove>(state->picker.entity_id, threshold);
            auto *material = state->scene.try_get<material_points>(state->picker.entity_id);
            if(material) {
                auto &position = material->attributes[0];
                position.property_name = "v_position";
                state->dispatcher.trigger<event::points_renderer::set_position_attribute>(state->picker.entity_id,
                                                                                          position);
                material->use_uniform_color = true;
                state->dispatcher.trigger<event::spatial_index::update_indices>(state->picker.entity_id);
            }
        }
    }
    ImGui::Separator();
    if (state->scene.valid(state->picker.entity_id)) {
        gui_material_points(state, state->scene.try_get<material_points>(state->picker.entity_id),
                            state->picker.entity_id);
    }
    ImGui::PopID();
}

}