//
// Created by alex on 01.03.21.
//

#include "bcg_gui_mesh_tutorial.h"
#include "viewer/bcg_viewer_state.h"
#include "../graphics/bcg_mesh_tutorial.h"

namespace bcg{

void gui_mesh_tutorial(viewer_state *state){
    ImGui::Separator();
    static mesh_tutorial_parameters parameters;
    float parameter_2 = parameters.parameter_2;
    float parameter_3 = parameters.parameter_3;

    ImGui::InputFloat("parameter_1", &parameters.parameter_1);
    ImGui::InputFloat("parameter_2", &parameter_2);
    ImGui::InputFloat("parameter_3", &parameter_3);
    ImGui::InputInt("iterations", &parameters.iterations);
    ImGui::InputInt("something_else", &parameters.something_else);

    if(ImGui::Button("Compute Method")){
        auto id = state->picker.entity_id;
        if(state->scene.valid(id)){
            if(state->scene.has<halfedge_mesh>(id)){
                auto &mesh = state->scene.get<halfedge_mesh>(id);
                parameters.parameter_2 = parameter_2;
                parameters.parameter_3 = parameter_3;
                mesh_geodesic_approx(mesh, parameters, state->config.parallel_grain_size);
            }
        }
    }
}

}