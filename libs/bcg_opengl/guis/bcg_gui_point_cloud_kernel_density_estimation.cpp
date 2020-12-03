//
// Created by alex on 03.12.20.
//

#include "bcg_gui_point_cloud_kernel_density_estimation.h"
#include "bcg_gui_kdtree_selector.h"
#include "renderers/points_renderer/bcg_material_points.h"
#include "renderers/points_renderer/bcg_events_points_renderer.h"
#include "bcg_viewer_state.h"

namespace bcg{

struct viewer_state;

void gui_point_cloud_kernel_density_estimation(viewer_state *state){
    auto parameters = gui_kd_tree_selector(state);
    if(ImGui::Button("Compute")){
        if(parameters.type == kdtree_parameters::Type::knn){
            state->dispatcher.trigger<event::point_cloud::vertex::kernel_density::knn>(state->picker.entity_id, parameters.num_closest);
        }else if(parameters.type == kdtree_parameters::Type::radius){
            state->dispatcher.trigger<event::point_cloud::vertex::kernel_density::radius>(state->picker.entity_id, parameters.radius);
        }

        auto *material = state->scene.try_get<material_points>(state->picker.entity_id);
        if(material){
            auto &color = material->attributes[1];
            color.property_name = "v_kernel_density";
            state->dispatcher.trigger<event::points_renderer::set_color_attribute>(state->picker.entity_id, color);
        }
    }
}

}