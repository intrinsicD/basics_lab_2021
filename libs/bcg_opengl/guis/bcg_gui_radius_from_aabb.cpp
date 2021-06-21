//
// Created by alex on 29.03.21.
//

#include "bcg_gui_radius_from_aabb.h"
#include "viewer/bcg_viewer_state.h"
#include "aligned_box/bcg_aligned_box_stats.h"

namespace bcg{

bcg_scalar_t gui_radius_from_aabb(viewer_state *state, bool *changed){
    static float radius = 0.001;
    static float percentage = 1;
    if(changed){
        *changed = false;
    }
    if(ImGui::InputFloat("radius", &radius, 0.001, 0.1, "%.6f")){
        if(changed){
            *changed = true;
        }
    }
    ImGui::InputFloat("percentage", &percentage);
    if(ImGui::Button("% from AABB diagonal")){
        if (state->scene.has<aligned_box3>(state->picker.entity_id)) {
            auto &aabb = state->scene.get<aligned_box3>(state->picker.entity_id);
            radius = aabb_stats::percentage_diagonal(aabb, percentage);
            if(changed){
                *changed = true;
            }
        }
    }
    return radius;
}

}