//
// Created by alex on 07.12.20.
//

#include "bcg_gui_registration.h"
#include "bcg_viewer_state.h"
#include "registration/bcg_registration.h"

namespace bcg {

void gui_registration(viewer_state *state) {
    static auto names = registration_names();
    static int e = 0;
    static int max_iters = 10;
    static entt::entity source_id = entt::null;
    static entt::entity target_id = entt::null;
    draw_combobox(&state->window, "Method", e, names);
    if (source_id != entt::null) {
        ImGui::LabelText("source_id", "%s", std::to_string(size_t(source_id)).c_str());
    }
    if (target_id != entt::null) {
        ImGui::LabelText("target_id", "%s", std::to_string(size_t(target_id)).c_str());
    }
    if (ImGui::Button("set source")) {
        source_id = state->picker.entity_id;
    }
    ImGui::SameLine();
    if (ImGui::Button("set target")) {
        target_id = state->picker.entity_id;
    }
    ImGui::Separator();
    ImGui::InputInt("max_iters", &max_iters);
    ImGui::Separator();
    if (ImGui::Button("align step")) {
        if(source_id != target_id && state->scene.valid(source_id) && state->scene.valid(target_id)){
            state->dispatcher.trigger<event::registration::align_step>(source_id, target_id,
                                                                       static_cast<RegistrationMethod>(e));
        }

    }
    ImGui::SameLine();
    static bool converge = false;
    ImGui::Checkbox("converge", &converge);
    if (ImGui::Button("align converge") || converge) {
        if(source_id != target_id && state->scene.valid(source_id) && state->scene.valid(target_id)) {
            state->dispatcher.trigger<event::registration::align_converge>(source_id, target_id,
                                                                           static_cast<RegistrationMethod>(e),
                                                                           max_iters);
            converge = true;
        }
    }
    if (ImGui::Button("reset")) {
        state->dispatcher.trigger<event::registration::reset>(source_id, target_id,
                                                              static_cast<RegistrationMethod>(e));
    }


    if(state->scene.valid(source_id) && state->scene.has<registration>(source_id)){
        ImGui::Separator();
        auto &reg = state->scene.get<registration>(source_id);
        draw_histogram(&state->window, "errors", reg.errors);
    }
}

}
