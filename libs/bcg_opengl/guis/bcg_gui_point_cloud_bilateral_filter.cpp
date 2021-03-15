//
// Created by alex on 14.12.20.
//

#include "bcg_gui_point_cloud_bilateral_filter.h"
#include "bcg_gui_reload_entity.h"
#include "bcg_gui_kdtree_selector.h"
#include "bcg_gui_material_points.h"
#include "viewer/bcg_viewer_state.h"
#include "renderers/points_renderer/bcg_material_points.h"
#include "renderers/points_renderer/bcg_events_points_renderer.h"

namespace bcg{

void gui_point_cloud_bilateral_filter(viewer_state *state){
    static float points_sigma = 0.01;
    static float normals_sigma = 0.01;
    kdtree_parameters params = gui_kd_tree_selector(state);
    ImGui::Separator();
    ImGui::InputFloat("points_sigma", &points_sigma);
    ImGui::InputFloat("normals_sigma", &normals_sigma);
    if(ImGui::Button("Compute Bilateral Filtering")){
        if(params.type == kdtree_parameters::Type::knn){
            state->dispatcher.trigger<event::point_cloud::vertex::filter::bilateral_knn>(state->picker.entity_id, points_sigma, normals_sigma, params.num_closest);
        }else if(params.type == kdtree_parameters::Type::radius){
            state->dispatcher.trigger<event::point_cloud::vertex::filter::bilateral_radius>(state->picker.entity_id, points_sigma, normals_sigma, params.radius);
        }
        state->scene.emplace_or_replace<event::points_renderer::enqueue>(state->picker.entity_id);
    }
    if(ImGui::Button("Use smooth version")){
        auto positions = state->get_vertices(state->picker.entity_id)->get<VectorS<3>, 3>("v_position");
        auto new_positions = state->get_vertices(state->picker.entity_id)->get<VectorS<3>, 3>("v_bilateral_smoothed_position");
        if(new_positions){
            positions.vector() = new_positions.vector();
            positions.set_dirty();
        }
    }

    gui_material_points(state, state->scene.try_get<material_points>(state->picker.entity_id), state->picker.entity_id);
    gui_reload_entity(state);
}

}