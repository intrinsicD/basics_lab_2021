//
// Created by alex on 01.12.20.
//

#include "bcg_gui_point_cloud_curvature_taubin.h"
#include "bcg_gui_point_cloud_vertex_pca.h"
#include "bcg_gui_material_points.h"
#include "bcg_viewer_state.h"
#include "renderers/points_renderer/bcg_material_points.h"

namespace bcg{

void gui_point_cloud_curvature_taubin(viewer_state *state){
    if(ImGui::CollapsingHeader("Vertex Pca")){
        gui_point_cloud_vertex_pca(state);
        ImGui::Separator();
    }
    static int num_closest = 12;
    static bcg_scalar_t radius = 0.001;
    static int e = 0;
    ImGui::RadioButton("knn query", &e, 0);
    ImGui::RadioButton("radius query", &e, 1);

    if(e == 0){
        ImGui::InputInt("num_closest", &num_closest);
    }else if(e == 1){
        ImGui::InputFloat("radius", &radius);
    }

    if(ImGui::Button("Compute")){
        if(e == 0){
            state->dispatcher.trigger<event::point_cloud::vertex::curvature::taubin_knn>(state->picker.entity_id, num_closest);
        }else if(e == 1){
            state->dispatcher.trigger<event::point_cloud::vertex::curvature::taubin_radius>(state->picker.entity_id, (bcg_scalar_t)radius);
        }

    }
    if (state->scene.valid(state->picker.entity_id)) {
        gui_material_points(state, state->scene.try_get<material_points>(state->picker.entity_id),
                            state->picker.entity_id);
    }
}

}