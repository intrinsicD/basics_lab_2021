//
// Created by alex on 09.01.21.
//

#include "bcg_gui_sampling_octree.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_gui_property_selector.h"
#include "geometry/sampling/bcg_sampling_octree.h"
#include "components/bcg_component_entity_hierarchy.h"
#include "renderers/picking_renderer/bcg_events_picking_renderer.h"
#include "math/vector/bcg_vector_map_eigen.h"
#include "entt/entt.hpp"
#include "utils/bcg_stl_utils.h"
#include <string>

namespace bcg {

using namespace entt::literals;

void gui_sampling_octree(viewer_state *state) {
    static int idx = 4;
    static int leaf_size = 1;
    static int max_depth = 20;
    static int sample_depth = 0;
    draw_combobox(&state->window, "sampling_type", idx, sampling_octree::type_names());
    ImGui::InputInt("leaf_size", &leaf_size);
    ImGui::InputInt("max_depth", &max_depth);
    bool trigger = false;
    static bool importance_sampling = false;
    static bool absolute = false;
    static std::string current_property_name;
    ImGui::Checkbox("importance sampling", &importance_sampling);
    ImGui::Checkbox("use absolute values", &absolute);
    if(importance_sampling){
        if (state->picker.valid) {
            auto *vertices = state->get_vertices(state->picker.entity_id);
            gui_property_selector(state, vertices,{1},"importance sampling",current_property_name);
        }
    }
    if (ImGui::Button("Compute Samples")) {
        if (state->picker.valid) {
            std::vector<size_t> indices;
            if(importance_sampling){
                auto *vertices = state->get_vertices(state->picker.entity_id);
                size_t size = vertices->size();
                indices.resize(size);
                for(size_t i = 0; i < size; ++i){
                    indices[i] = i;
                }
                auto prop = vertices->get<bcg_scalar_t, 1>(current_property_name);
                std::vector<bcg_scalar_t> values = prop.vector();
                if(absolute){
                    Map(values) = Map(prop.vector()).cwiseAbs();
                }
                sort_by_first(values, indices);
            }
            state->dispatcher.trigger<event::spatial_index::setup_sampling_octree>(state->picker.entity_id,
                                                                                   static_cast<sampling_octree::SamplingType>(idx),
                                                                                   indices,
                                                                                   static_cast<size_t>(leaf_size), max_depth);
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
            auto &hierarchy = state->scene().get_or_emplace<entity_hierarchy>(state->picker.entity_id);
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
                state->scene().emplace<point_cloud>(id, pc);
                state->scene().emplace<entt::tag<"subsampled"_hs>>(id);
                state->dispatcher.trigger<event::internal::entity_setup>(id, false);
                state->dispatcher.trigger<event::point_cloud::setup>(id, "subsampled");
                state->scene().remove_if_exists<event::picking_renderer::enqueue>(id);
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