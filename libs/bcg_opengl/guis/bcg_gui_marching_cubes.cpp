//
// Created by alex on 25.11.20.
//

#include "geometry/marching_cubes/bcg_marching_cubes.h"
#include "bcg_gui_marching_cubes.h"
#include "bcg_viewer_state.h"

namespace bcg{

void gui_marching_cubes(viewer_state *state){
    static VectorS<3> min = -2 * VectorS<3>::Ones();
    static VectorS<3> max = 2 * VectorS<3>::Ones();
    static Vector<int, 3> dims = {30, 30, 30};
    static bcg_scalar_t isovalue = 0;
    ImGui::InputFloat3("min", min.data());
    ImGui::InputFloat3("max", max.data());
    ImGui::InputInt3("dims", dims.data());
    marching_cubes mc;
    mc.function = marching_cubes::hearts_function;
    if(ImGui::Button("convert to mesh")){
        auto mesh = mc.reconstruct(0, min, max, dims.cast<bcg_index_t>());
        auto id = state->scene.create();
        state->scene.emplace<halfedge_mesh>(id, mesh);
        state->dispatcher.trigger<event::mesh::setup>(id, "marching cubes");
    }
}

}