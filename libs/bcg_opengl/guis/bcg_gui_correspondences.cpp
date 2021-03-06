//
// Created by alex on 07.12.20.
//

#include "bcg_gui_correspondences.h"
#include "viewer/bcg_viewer_state.h"
#include "correspondences/bcg_correspondences.h"
#include "bcg_gui_statistics.h"
#include "bcg_gui_vectorfields.h"
#include "renderers/vectorfield_renderer/bcg_events_vectorfield_renderer.h"
#include "math/vector/bcg_vector_map_eigen.h"
#include "bcg_gui_statistics.h"
#include "math/vector/bcg_vector_map_eigen.h"

namespace bcg {

void gui_correspondence_stats(viewer_state *state, const correspondences &corrs){
    ImGui::LabelText("target_id", "%zu",corrs.target_id);
    ImGui::LabelText("num_corrs", "%zu", corrs.mapping.size());
    ImGui::LabelText("max_source_index", "%zu", corrs.max_source_index);
    ImGui::LabelText("max_target_index", "%zu", corrs.max_target_index);
    ImGui::Separator();
    std::vector<float> values(corrs.mapping.size());
    Map(values) = MapConst(corrs.weights()).cast<float>();
    std::sort(values.begin(), values.end());
    gui_statistics(state, &corrs.stats, &values);
}

void gui_correspondences(viewer_state *state) {
    static correspondences *corrs;
    static entt::entity source_id = entt::null;
    static entt::entity target_id = entt::null;

    if (state->scene.valid(source_id)) {
        auto &entity_corrs = state->scene().get_or_emplace<entity_correspondences>(source_id);
        if (state->scene.valid(target_id)) {
            corrs = &entity_corrs.maps[size_t(target_id)];
            corrs->target_id = size_t(target_id);
        }
    }
    if (source_id != entt::null) {
        ImGui::LabelText("source_id", "%s", std::to_string(size_t(source_id)).c_str());
    }
    if (corrs != nullptr && corrs->target_id != BCG_INVALID_ID) {
        ImGui::LabelText("target_id", "%s", std::to_string(size_t(corrs->target_id)).c_str());
    }
    if (ImGui::Button("set source")) {
        if (state->scene.valid(state->picker.entity_id)) {
            source_id = state->picker.entity_id;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("set target")) {
        target_id = state->picker.entity_id;
    }
    ImGui::Separator();
    static float distance_threshold = 0;
    static float angle_threshold = 45;
    static bool estimate_every_frame = false;
    bool estimate_corrs = false;
    bool filter_distance = false;
    bool filter_normal_angle = false;
    static bool filter_distance_every_frame = false;
    static bool filter_normal_angle_every_frame = false;

    ImGui::Checkbox("estimate_every_frame", &estimate_every_frame);
    if(estimate_every_frame){
        ImGui::Checkbox("filter_distance_every_frame", &filter_distance_every_frame);
        ImGui::Checkbox("filter_normal_angle_every_frame", &filter_normal_angle_every_frame);
    }

    if (ImGui::Button("estimate") || estimate_every_frame) {
        estimate_corrs = true;
    }
    if (corrs != nullptr && !corrs->mapping.empty()) {
        ImGui::Separator();

        ImGui::InputFloat("threshold dist", &distance_threshold);
        ImGui::SameLine();
        if (ImGui::Button("filter distance")) {
            filter_distance = true;
        }
        ImGui::InputFloat("threshold angle", &angle_threshold);
        ImGui::SameLine();
        if (ImGui::Button("filter angle")) {
            filter_normal_angle = true;
        }
        auto *vfs = state->scene.try_get<vectorfields>(source_id);
        gui_vectorfields(state, vfs, source_id);
    }

    if (estimate_corrs || estimate_every_frame) {
        state->dispatcher.trigger<event::correspondences::estimate>(source_id, entt::entity(corrs->target_id));
        if(estimate_corrs){
            distance_threshold = corrs->stats.mean();
        }
        state->dispatcher.trigger<event::vectorfield_renderer::set_vertex_vectorfield>(source_id, "v_corrs_vector");
    }

    if (filter_distance || filter_distance_every_frame) {
        if (corrs != nullptr) {
            state->dispatcher.trigger<event::correspondences::filter::distance>(source_id,
                                                                                entt::entity(corrs->target_id),
                                                                                distance_threshold);
        }
    }

    if (filter_normal_angle || filter_normal_angle_every_frame) {
        if (corrs != nullptr) {
            state->dispatcher.trigger<event::correspondences::filter::normal_angle>(source_id,
                                                                                    entt::entity(corrs->target_id),
                                                                                    angle_threshold);
        }
    }

    if (corrs != nullptr && !corrs->mapping.empty() && ImGui::CollapsingHeader("statistics")) {
        gui_correspondence_stats(state, *corrs);
        ImGui::Separator();
    }

}

}