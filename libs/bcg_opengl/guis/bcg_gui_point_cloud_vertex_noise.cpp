//
// Created by alex on 23.02.21.
//

#include "bcg_gui_point_cloud_vertex_noise.h"
#include "bcg_gui_reload_entity.h"
#include "viewer/bcg_viewer_state.h"
#include "geometry/point_cloud/bcg_point_cloud_vertex_noise.h"

namespace bcg{

bool gui_point_cloud_vertex_noise(viewer_state *state){
    static bcg_scalar_t noise_level = 1;
    draw_input(&state->window, "noise level %diag aabb", noise_level);
    bool changed = false;
    if(ImGui::Button("Add noise")){
        auto id = state->picker.entity_id;
        if(state->scene.valid(id)){
            auto *vertices = state->get_vertices(id);
            if(vertices){
                auto &aabb = state->scene.get<aligned_box3>(id);
                point_cloud_vertex_noise(vertices, aabb, noise_level, state->config.parallel_grain_size);
                changed = true;
            }
        }
    }
    ImGui::Separator();
    gui_reload_entity(state);
    return changed;
}

}