//
// Created by alex on 09.01.21.
//

#include "bcg_gui_octree_sampling.h"
#include "bcg_viewer_state.h"
#include "geometry/sampling/bcg_sampling_octree.h"
#include "bcg_entity_hierarchy.h"
#include "entt/entt.hpp"
#include <string>

namespace bcg {

using namespace entt::literals;

void gui_octree_sampling(viewer_state *state) {
    static int idx = 4;
    static int leaf_size = 1;
    static int max_depth = 20;
    static int sample_depth = 0;
    draw_combobox(&state->window, "sampling_type", idx, sampling_octree::type_names());
    ImGui::InputInt("leaf_size", &leaf_size);
    ImGui::InputInt("max_depth", &max_depth);
    bool trigger = false;
    if (ImGui::Button("Compute Samples")) {
        if (state->picker.valid) {
            state->dispatcher.trigger<event::spatial_index::setup_sampling_octree>(state->picker.entity_id,
                                                                                   static_cast<sampling_octree::SamplingType>(idx),
                                                                                   leaf_size, max_depth);
            auto &index = state->scene.get<sampling_octree>(state->picker.entity_id);
            max_depth = index.current_depth;
            sample_depth = index.current_depth / 2;
            trigger = true;
        }
    }
    if(state->scene.valid(state->picker.entity_id) && state->scene.has<sampling_octree>(state->picker.entity_id)){
        if(ImGui::Button("+")){
            ++sample_depth;
            trigger = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("-") && sample_depth > 0){
            --sample_depth;
            trigger = true;
        }

        if(ImGui::InputInt("sample_depth", &sample_depth)){
            trigger = true;
        }
        if(trigger){
            sample_depth = std::max<int>(sample_depth, 0);
            sample_depth = std::min<int>(sample_depth, max_depth);
            auto &index = state->scene.get<sampling_octree>(state->picker.entity_id);
            auto &hierarchy = state->scene.get_or_emplace<entity_hierarchy>(state->picker.entity_id);
            entt::entity child_id = entt::null;
            for (const auto &child : hierarchy.children) {
                if (state->scene.has<entt::tag<"subsampled"_hs>>(child.first)) {
                    child_id = child.first;
                    break;
                }
            }

            if(child_id == entt::null){
                auto parent_id = state->picker.entity_id;
                point_cloud pc;
                pc.positions.set(index.get_samples(sample_depth));
                pc.vertices.resize(pc.positions.size());
                pc.positions.set_dirty();
                auto id = state->scene.create();
                state->scene.emplace<point_cloud>(id, pc);
                state->scene.emplace<entt::tag<"subsampled"_hs>>(id);
                state->dispatcher.trigger<event::point_cloud::setup>(id, "subsampled");
                state->dispatcher.trigger<event::hierarchy::add_child>(parent_id, id);
                state->picker.entity_id = parent_id;
            }else{
                auto &pc = state->scene.get<point_cloud>(child_id);
                pc.positions.set(index.get_samples(sample_depth));
                pc.vertices.resize(pc.positions.size());
                pc.positions.set_dirty();
            }
        }
    }
}

}