//
// Created by alex on 01.12.20.
//

#include "bcg_gui_point_cloud_curvature_taubin.h"
#include "bcg_gui_point_cloud_vertex_pca.h"
#include "bcg_gui_material_points.h"
#include "bcg_gui_kdtree_selector.h"
#include "bcg_viewer_state.h"
#include "renderers/points_renderer/bcg_material_points.h"

namespace bcg{

void gui_point_cloud_curvature_taubin(viewer_state *state){
    if(ImGui::CollapsingHeader("Vertex Pca")){
        gui_point_cloud_vertex_pca(state);
        ImGui::Separator();
    }

    ImGui::PushID("pc_curvature_taubin");
    auto parameters = gui_kd_tree_selector(state);

    if(ImGui::Button("Compute")){
        if(parameters.type == kdtree_parameters::Type::knn){
            state->dispatcher.trigger<event::point_cloud::vertex::curvature::taubin_knn>(state->picker.entity_id, parameters.num_closest);
        }else if(parameters.type == kdtree_parameters::Type::radius){
            state->dispatcher.trigger<event::point_cloud::vertex::curvature::taubin_radius>(state->picker.entity_id, parameters.radius);
        }
    }
    if (state->scene.valid(state->picker.entity_id)) {
        gui_material_points(state, state->scene.try_get<material_points>(state->picker.entity_id),
                            state->picker.entity_id);
    }
    ImGui::PopID();
}

}