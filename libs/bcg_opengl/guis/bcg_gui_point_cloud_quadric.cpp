//
// Created by alex on 03.12.20.
//

#include "bcg_gui_point_cloud_quadric.h"
#include "bcg_gui_kdtree_selector.h"
#include "bcg_viewer_state.h"
#include "renderers/points_renderer/bcg_events_points_renderer.h"
#include "renderers/points_renderer/bcg_material_points.h"
#include "geometry/point_cloud/bcg_point_cloud_vertex_quadric.h"

namespace bcg{

void gui_point_cloud_quadric(viewer_state *state){
    static std::vector<std::string> names = quadric_names();
    static int e = 0;
    draw_combobox(&state->window, "quadric type", e, names);
    kdtree_parameters parameters = gui_kd_tree_selector(state);
    static bool can_collect = false;

    if(ImGui::Button("Compute Quadric")){
        switch (static_cast<PointCloudQuadricTypes>(e)){
            case PointCloudQuadricTypes::point : {
                state->dispatcher.trigger<event::point_cloud::vertex::quadric::point>(state->picker.entity_id);
                break;
            }
            case PointCloudQuadricTypes::plane : {
                state->dispatcher.trigger<event::point_cloud::vertex::quadric::plane>(state->picker.entity_id);
                break;
            }
            case PointCloudQuadricTypes::isotropic : {
                if(parameters.type == kdtree_parameters::Type::knn){
                    state->dispatcher.trigger<event::point_cloud::vertex::quadric::probabilistic_plane_quadric_isotropic_knn>(state->picker.entity_id, parameters.num_closest);
                }else if(parameters.type == kdtree_parameters::Type::radius){
                    state->dispatcher.trigger<event::point_cloud::vertex::quadric::probabilistic_plane_quadric_isotropic_radius>(state->picker.entity_id, parameters.radius);
                }
                break;
            }
            case PointCloudQuadricTypes::anisotropic : {
                if(parameters.type == kdtree_parameters::Type::knn){
                    state->dispatcher.trigger<event::point_cloud::vertex::quadric::probabilistic_plane_quadric_anisotropic_knn>(state->picker.entity_id, parameters.num_closest);
                }else if(parameters.type == kdtree_parameters::Type::radius){
                    state->dispatcher.trigger<event::point_cloud::vertex::quadric::probabilistic_plane_quadric_anisotropic_radius>(state->picker.entity_id, parameters.radius);
                }
                break;
            }
            case PointCloudQuadricTypes::__last__ : {

            }
            default :{

            }
        }
        can_collect = true;
    }

    if(can_collect){
        if(ImGui::Button("Collect (Neighborhood quadrics)")){
            state->dispatcher.trigger<event::point_cloud::vertex::quadric::collect>(state->picker.entity_id);
            auto *material = state->scene.try_get<material_points>(state->picker.entity_id);
            if(material){
                auto &color = material->attributes[1];
                color.property_name = "v_quadric_error";
                state->dispatcher.trigger<event::points_renderer::set_color_attribute>(state->picker.entity_id, color);
            }
            can_collect = false;
        }
    }
}

}