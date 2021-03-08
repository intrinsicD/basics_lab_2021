//
// Created by alex on 01.03.21.
//

#include "bcg_gui_mesh_normal_filtering.h"
#include "bcg_opengl/bcg_viewer_state.h"
#include "bcg_mesh_normal_filtering.h"
#include "guis/bcg_gui_point_cloud_vertex_noise.h"

namespace bcg{

void gui_mesh_normal_filtering(viewer_state *state){
    gui_point_cloud_vertex_noise(state);
    ImGui::Separator();
    static float sigma_f = 0.1;
    static float sigma_g = 0.1;
    static int iterations = 1;
    static int something_else = 1;

    ImGui::InputFloat("sigma_f", &sigma_f);
    ImGui::InputFloat("sigma_g", &sigma_g);
    ImGui::InputInt("iterations", &iterations);
    ImGui::InputInt("something_else", &something_else);

    if(ImGui::Button("Compute Method")){
        auto id = state->picker.entity_id;
        if(state->scene.valid(id)){
            if(state->scene.has<halfedge_mesh>(id)){
                auto &mesh = state->scene.get<halfedge_mesh>(id);
                mesh_nf_parameters params{bcg_scalar_t(sigma_f), bcg_scalar_t(sigma_g), iterations, something_else};
                mesh_normal_filtering(mesh, params, state->config.parallel_grain_size);
            }
        }
    }
}

}