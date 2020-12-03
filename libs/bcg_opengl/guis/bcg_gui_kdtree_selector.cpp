//
// Created by alex on 03.12.20.
//

#include "bcg_gui_kdtree_selector.h"
#include "bcg_viewer_state.h"

namespace bcg{

kdtree_parameters gui_kd_tree_selector(viewer_state *){
    static int num_closest = 12;
    static float radius = 0.001;
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
            num_closest = parameters.num_closest;
        }
    }else if(e == 1){
        parameters.type = kdtree_parameters::Type::radius;
        parameters.radius = radius;
        if(ImGui::InputFloat("radius", &radius)){
            num_closest = parameters.num_closest;
        }
    }
    ImGui::PopID();
    return parameters;
}

}