//
// Created by alex on 07.12.20.
//

#include "bcg_gui_registration.h"
#include "bcg_viewer_state.h"
#include "bcg_gui_transform.h"
#include "registration/bcg_registration.h"
#include "registration/coherent_point_drift/bcg_coherent_point_drift_rigid.h"
#include "registration/coherent_point_drift/bcg_coherent_point_drift_affine.h"
#include "registration/coherent_point_drift/bcg_coherent_point_drift_nonrigid.h"
#include "registration/coherent_point_drift/bcg_coherent_point_drift_bayes.h"
#include "registration/coherent_point_drift/bcg_coherent_point_drift_test.h"

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
        if (source_id != target_id && state->scene.valid(source_id) && state->scene.valid(target_id)) {
            state->dispatcher.trigger<event::registration::align_step>(source_id, target_id,
                                                                       static_cast<RegistrationMethod>(e));
        }

    }
    ImGui::SameLine();
    static bool converge = false;
    ImGui::Checkbox("converge", &converge);
    if (ImGui::Button("align converge") || converge) {
        if (source_id != target_id && state->scene.valid(source_id) && state->scene.valid(target_id)) {
            state->dispatcher.trigger<event::registration::align_converge>(source_id, target_id,
                                                                           static_cast<RegistrationMethod>(e),
                                                                           max_iters);
            converge = true;
        }
    }
    if (ImGui::Button("reset")) {
        state->dispatcher.trigger<event::registration::reset>(source_id, target_id, static_cast<RegistrationMethod>(e));
    }


    if (state->scene.valid(source_id) && state->scene.has<registration>(source_id)) {
        ImGui::Separator();
        auto &reg = state->scene.get<registration>(source_id);
        draw_histogram(&state->window, "errors", reg.errors);
    }

    static auto names_p = names_type_p();
    static int ptype = 0;
    static int num_samples = 10;
    if (state->scene.valid(source_id) && ImGui::CollapsingHeader("Info")) {
        switch (static_cast<RegistrationMethod>(e)) {
            case RegistrationMethod::rigid_icp_point2point : {
                break;
            }
            case RegistrationMethod::rigid_icp_point2plane : {
                break;
            }
            case RegistrationMethod::coherent_point_drift_rigid : {
                auto &rigid = state->scene.get_or_emplace<coherent_point_drift_rigid>(source_id);
                ImGui::LabelText("sigma_squared", "%s", std::to_string(rigid.sigma_squared).c_str());
                ImGui::LabelText("N_P", "%s", std::to_string(rigid.N_P).c_str());
                ImGui::InputFloat("omega", &rigid.omega);
                draw_combobox(&state->window, "type P", ptype, names_p);
                rigid.p_type = static_cast<coherent_point_drift_base::TypeP>(ptype);
                if (rigid.p_type == coherent_point_drift_base::TypeP::nyström) {
                    ImGui::InputInt("num_samples", &num_samples);
                    rigid.num_samples = num_samples;
                }
                break;
            }
            case RegistrationMethod::coherent_point_drift_affine : {
                auto &affine = state->scene.get_or_emplace<coherent_point_drift_affine>(source_id);
                ImGui::LabelText("sigma_squared", "%s", std::to_string(affine.sigma_squared).c_str());
                ImGui::LabelText("N_P", "%s", std::to_string(affine.N_P).c_str());
                ImGui::InputFloat("omega", &affine.omega);
                draw_combobox(&state->window, "type P", ptype, names_p);
                affine.p_type = static_cast<coherent_point_drift_base::TypeP>(ptype);
                if (affine.p_type == coherent_point_drift_base::TypeP::nyström) {
                    ImGui::InputInt("num_samples", &num_samples);
                    affine.num_samples = num_samples;
                }
                break;
            }
            case RegistrationMethod::coherent_point_drift_nonrigid : {
                auto &nonrigid = state->scene.get_or_emplace<coherent_point_drift_nonrigid>(source_id);
                ImGui::LabelText("sigma_squared", "%s", std::to_string(nonrigid.sigma_squared).c_str());
                ImGui::LabelText("N_P", "%s", std::to_string(nonrigid.N_P).c_str());
                ImGui::InputFloat("omega", &nonrigid.omega);
                ImGui::InputFloat("beta", &nonrigid.beta);
                ImGui::InputFloat("lambda", &nonrigid.lambda);
                draw_combobox(&state->window, "type P", ptype, names_p);
                nonrigid.p_type = static_cast<coherent_point_drift_base::TypeP>(ptype);
                if (nonrigid.p_type == coherent_point_drift_base::TypeP::nyström) {
                    ImGui::InputInt("num_samples", &num_samples);
                    nonrigid.num_samples = num_samples;
                }
                break;
            }
            case RegistrationMethod::coherent_point_drift_bayes : {
                auto &bayes = state->scene.get_or_emplace<coherent_point_drift_bayes>(source_id);
                ImGui::LabelText("sigma_squared", "%s", std::to_string(bayes.sigma_squared).c_str());
                ImGui::LabelText("N_P", "%s", std::to_string(bayes.N_P).c_str());
                ImGui::InputFloat("omega", &bayes.omega);
                ImGui::InputFloat("beta", &bayes.beta);
                ImGui::InputFloat("gamma", &bayes.gamma);
                ImGui::InputFloat("kappa", &bayes.kappa);
                ImGui::InputFloat("lambda", &bayes.lambda);
                break;
            }
            case RegistrationMethod::coherent_point_drift_test : {
                auto &nonrigid_test = state->scene.get_or_emplace<coherent_point_drift_test>(source_id);
                ImGui::LabelText("sigma_squared", "%s", std::to_string(nonrigid_test.sigma_squared).c_str());
                ImGui::LabelText("N_P", "%s", std::to_string(nonrigid_test.N_P).c_str());
                ImGui::InputFloat("omega", &nonrigid_test.omega);
                ImGui::InputFloat("beta", &nonrigid_test.beta);
/*                ImGui::InputFloat("gamma", &bayes.gamma);
                ImGui::InputFloat("kappa", &bayes.kappa);*/
                ImGui::InputFloat("lambda", &nonrigid_test.lambda);
                draw_combobox(&state->window, "type P", ptype, names_p);
                nonrigid_test.p_type = static_cast<coherent_point_drift_base::TypeP>(ptype);
                if (nonrigid_test.p_type == coherent_point_drift_base::TypeP::nyström) {
                    ImGui::InputInt("num_samples", &num_samples);
                    nonrigid_test.num_samples = num_samples;
                }
                break;
            }
            case RegistrationMethod::__last__: {
                break;
            }
            default : {
                break;
            }
        }
        ImGui::Separator();
        gui_transform(state, state->scene.try_get<Transform>(source_id));
    }
}

}
