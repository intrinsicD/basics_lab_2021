//
// Created by alex on 03.12.20.
//

#include "bcg_gui_kdtree_selector.h"
#include "bcg_viewer_state.h"
#include "aligned_box/bcg_aligned_box.h"
#include "aligned_box/bcg_aligned_box_stats.h"

namespace bcg{

kdtree_parameters gui_kd_tree_selector(viewer_state *state){
    static int num_closest = 12;
    static float radius = 0.001;
    static float percentage = 1;
    static kdtree_parameters parameters;
    static int e = 0;
    ImGui::PushID("kdtree_selector");
    ImGui::RadioButton("knn query", &e, 0);
    ImGui::RadioButton("radius query", &e, 1);

    if(e == 0){
        parameters.type = kdtree_parameters::Type::knn;
        parameters.num_closest = num_closest;
        if(ImGui::InputInt("num_closest", &parameters.num_closest)){
            radius = parameters.radius;
            parameters.radius = 0;
            num_closest = parameters.num_closest;
        }
    }else if(e == 1){
        parameters.type = kdtree_parameters::Type::radius;
        parameters.radius = radius;
        if(ImGui::InputFloat("radius", &radius, 0.001, 0.1, "%.6f")){
            num_closest = parameters.num_closest;
            parameters.num_closest = 0;
        }
        ImGui::InputFloat("percentage", &percentage);
        if(ImGui::Button("% from AABB diagonal")){
            if (state->scene.has<aligned_box3>(state->picker.entity_id)) {
                auto *vertices = state->get_vertices(state->picker.entity_id);
                if(vertices){
                    auto &aabb = state->scene.get<aligned_box3>(state->picker.entity_id);
                    radius = aabb_stats::percentage_diagonal(aabb, percentage);
                }
            }
        }
    }
    ImGui::PopID();
    return parameters;
}

}