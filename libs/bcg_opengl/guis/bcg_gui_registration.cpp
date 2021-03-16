//
// Created by alex on 07.12.20.
//

#include "bcg_gui_registration.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_gui_transform.h"
#include "bcg_gui_kernel_matrix.h"
#include "registration/bcg_registration.h"
#include "registration/bcg_coherent_point_drift.h"

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


    if (state->scene.valid(source_id) && state->scene.all_of<registration>(source_id)) {
        ImGui::Separator();
        auto &reg = state->scene.get<registration>(source_id);
        draw_histogram(&state->window, "errors", reg.errors);
    }

    static auto names_softmatching = softmatching_type_names();
    static auto names_sampling = sampling_type_names();
    static auto names_coherence = coherence_type_names();
    static auto names_kernel = kernel_type_names();

    static int softmatching_type = 0;
    static int sampling_type = 0;
    static int coherence_type = 0;
    static int coherence_kernel_type = 0;

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
                draw_input(&state->window, "omega", rigid.omega);
                ImGui::Checkbox("debug", &rigid.debug_output);

                rigid.kernel_P.kernel_type = KernelType::gaussian;

                draw_combobox(&state->window, "softmatching", softmatching_type, names_softmatching);
                rigid.softmatching_type = static_cast<SoftmatchingType>(softmatching_type);
                if (rigid.softmatching_type == SoftmatchingType::nystroem || rigid.softmatching_type == SoftmatchingType::nystroem_FGT) {
                    draw_combobox(&state->window, "sampling", sampling_type, names_sampling);
                    ImGui::InputInt("num_samples", &num_samples);
                    rigid.kernel_P.sampling_type = static_cast<SamplingType>(sampling_type);
                    rigid.num_samples = num_samples;
                }
                draw_histogram(&state->window, "likelihood", rigid.likelihood);
                break;
            }
            case RegistrationMethod::coherent_point_drift_affine : {
                auto &affine = state->scene.get_or_emplace<coherent_point_drift_affine>(source_id);
                ImGui::LabelText("sigma_squared", "%s", std::to_string(affine.sigma_squared).c_str());
                ImGui::LabelText("N_P", "%s", std::to_string(affine.N_P).c_str());
                draw_input(&state->window,"omega", affine.omega);
                ImGui::Checkbox("debug", &affine.debug_output);

                affine.kernel_P.kernel_type = KernelType::gaussian;

                draw_combobox(&state->window, "softmatching", softmatching_type, names_softmatching);
                affine.softmatching_type = static_cast<SoftmatchingType>(softmatching_type);
                if (affine.softmatching_type == SoftmatchingType::nystroem || affine.softmatching_type == SoftmatchingType::nystroem_FGT) {
                    draw_combobox(&state->window, "sampling", sampling_type, names_sampling);
                    ImGui::InputInt("num_samples", &num_samples);
                    affine.kernel_P.sampling_type = static_cast<SamplingType>(sampling_type);
                    affine.num_samples = num_samples;
                }
                draw_histogram(&state->window, "likelihood", affine.likelihood);
                break;
            }
            case RegistrationMethod::coherent_point_drift_nonrigid : {
                auto &nonrigid = state->scene.get_or_emplace<coherent_point_drift_nonrigid>(source_id);
                ImGui::LabelText("sigma_squared", "%s", std::to_string(nonrigid.sigma_squared).c_str());
                ImGui::LabelText("N_P", "%s", std::to_string(nonrigid.N_P).c_str());
                draw_input(&state->window,"omega", nonrigid.omega);
                draw_input(&state->window,"beta", nonrigid.beta);
                draw_input(&state->window,"lambda", nonrigid.lambda);
                ImGui::Checkbox("debug", &nonrigid.debug_output);

                nonrigid.kernel_P.kernel_type = KernelType::gaussian;

                draw_combobox(&state->window, "softmatching", softmatching_type, names_softmatching);
                nonrigid.softmatching_type = static_cast<SoftmatchingType>(softmatching_type);
                if (nonrigid.softmatching_type == SoftmatchingType::nystroem || nonrigid.softmatching_type == SoftmatchingType::nystroem_FGT) {
                    draw_combobox(&state->window, "sampling", sampling_type, names_sampling);
                    ImGui::InputInt("num_samples", &num_samples);
                    nonrigid.kernel_P.sampling_type = static_cast<SamplingType>(sampling_type);
                    nonrigid.num_samples = num_samples;
                }
                draw_combobox(&state->window, "coherence", coherence_type, names_coherence);
                nonrigid.coherence_type = static_cast<CoherenceType>(coherence_type);
                draw_combobox(&state->window, "coherence_kernel", coherence_kernel_type, names_kernel);
                nonrigid.kernel_G.kernel_type = static_cast<KernelType>(coherence_kernel_type);
                ImGui::InputInt("num_evals", &nonrigid.num_evals);
                draw_histogram(&state->window, "likelihood", nonrigid.likelihood);
                break;
            }
            case RegistrationMethod::coherent_point_drift_nonrigid2 : {
                auto &nonrigid = state->scene.get_or_emplace<coherent_point_drift_nonrigid2>(source_id);
                ImGui::LabelText("sigma_squared", "%s", std::to_string(nonrigid.sigma_squared).c_str());
                ImGui::LabelText("N_P", "%s", std::to_string(nonrigid.N_P).c_str());
                draw_input(&state->window,"omega", nonrigid.omega);
                draw_input(&state->window,"beta", nonrigid.beta);
                draw_input(&state->window,"lambda", nonrigid.lambda);
                ImGui::Checkbox("debug", &nonrigid.debug_output);

                nonrigid.kernel_P.kernel_type = KernelType::gaussian;

                draw_combobox(&state->window, "softmatching", softmatching_type, names_softmatching);
                nonrigid.softmatching_type = static_cast<SoftmatchingType>(softmatching_type);
                if (nonrigid.softmatching_type == SoftmatchingType::nystroem || nonrigid.softmatching_type == SoftmatchingType::nystroem_FGT) {
                    draw_combobox(&state->window, "sampling", sampling_type, names_sampling);
                    ImGui::InputInt("num_samples", &num_samples);
                    nonrigid.kernel_P.sampling_type = static_cast<SamplingType>(sampling_type);
                    nonrigid.num_samples = num_samples;
                }
                draw_combobox(&state->window, "coherence", coherence_type, names_coherence);
                nonrigid.coherence_type = static_cast<CoherenceType>(coherence_type);
                draw_combobox(&state->window, "coherence_kernel", coherence_kernel_type, names_kernel);
                nonrigid.kernel_G.kernel_type = static_cast<KernelType>(coherence_kernel_type);
                ImGui::InputInt("num_evals", &nonrigid.num_evals);
                draw_histogram(&state->window, "likelihood", nonrigid.likelihood);
                break;
            }
            case RegistrationMethod::coherent_point_drift_bayes : {
                auto &bayes = state->scene.get_or_emplace<coherent_point_drift_bayes>(source_id);
                ImGui::LabelText("sigma_squared", "%s", std::to_string(bayes.sigma_squared).c_str());
                ImGui::LabelText("N_P", "%s", std::to_string(bayes.N_P).c_str());
                ImGui::LabelText("scale", "%s", std::to_string(bayes.s).c_str());
                draw_input(&state->window,"omega", bayes.omega);
                draw_input(&state->window,"beta", bayes.beta);
                draw_input(&state->window,"lambda", bayes.lambda);
                draw_input(&state->window,"gamma", bayes.gamma);
                draw_input(&state->window,"kappa", bayes.kappa);
                ImGui::Checkbox("debug", &bayes.debug_output);

                bayes.kernel_P.kernel_type = KernelType::gaussian;

                draw_combobox(&state->window, "softmatching", softmatching_type, names_softmatching);
                bayes.softmatching_type = static_cast<SoftmatchingType>(softmatching_type);
                if (bayes.softmatching_type == SoftmatchingType::nystroem || bayes.softmatching_type == SoftmatchingType::nystroem_FGT) {
                    draw_combobox(&state->window, "sampling", sampling_type, names_sampling);
                    ImGui::InputInt("num_samples", &num_samples);
                    bayes.kernel_P.sampling_type = static_cast<SamplingType>(sampling_type);
                    bayes.num_samples = num_samples;
                }
                draw_combobox(&state->window, "coherence", coherence_type, names_coherence);
                bayes.coherence_type = static_cast<CoherenceType>(coherence_type);
                draw_combobox(&state->window, "coherence_kernel", coherence_kernel_type, names_kernel);
                bayes.kernel_G.kernel_type = static_cast<KernelType>(coherence_kernel_type);
                ImGui::InputInt("num_evals", &bayes.num_evals);
                draw_histogram(&state->window, "likelihood", bayes.likelihood);
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
