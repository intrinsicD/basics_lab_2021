//
// Created by alex on 22.04.21.
//

#include "bcg_gui_rotation_averaging_chordal_mean.h"
#include "viewer/bcg_viewer_state.h"
#include "math/rotations/bcg_rotation_chordal_mean.h"

namespace bcg{

bool gui_rotation_averaging_cordal_mean(viewer_state *state, MatrixS<3, 3> &result){
    static bool closed_form = false;
    static bool reject_outlier = false;
    bool update = false;
    struct Sources{
        Sources(entt::entity id, bool selected) : id(id), selected(selected){}
        entt::entity id;
        bool selected;
    };

    static std::vector<Sources> sources;
    if(ImGui::Button("Selection as sources")){
        sources.clear();
        for(const auto &item : state->picker.selected_entities){
            sources.emplace_back(item.second, true);
        }
    }
    for(auto &source: sources){
        ImGui::Checkbox(("Select " + std::to_string(int(source.id))).c_str(), &source.selected);
    }

    if(ImGui::Button("Compute Chordal Mean")){
        chordal_mean_so3 method(closed_form, reject_outlier);
        std::vector<MatrixS<3, 3>> rotations;
        for(const auto &source: sources){
            auto model = state->scene.get<Transform>(source.id);
            rotations.emplace_back(model.linear());
            result = method(rotations);
            update = true;
        }
    }
    return update;
}

}