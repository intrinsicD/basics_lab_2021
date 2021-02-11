//
// Created by alex on 09.02.21.
//


#include "bcg_gui_sampling_grid.h"
#include "bcg_viewer_state.h"
#include "bcg_entity_hierarchy.h"
#include "renderers/picking_renderer/bcg_events_picking_renderer.h"
#include "geometry/sampling/bcg_sampling_grid.h"

namespace bcg {

using namespace entt::literals;

void gui_sampling_grid(viewer_state *state) {
    static auto sampling_type_names = grid_sampling_type_names();
    static int idx = 0;
    draw_combobox(&state->window, "sampling type", idx, sampling_type_names);
    static Vector<int, 3> dims = Vector<int, 3>::Ones() * 10;
    ImGui::InputInt3("dims", dims.data());
    if (ImGui::Button("Compute Sampling")) {
        if (state->scene.valid(state->picker.entity_id)) {
            auto id = state->picker.entity_id;
            auto &aabb = state->scene.get<aligned_box3>(id);
            auto *vertices = state->get_vertices(id);
            if (vertices) {
                auto positions = vertices->get<VectorS<3>, 3>("v_position");
                std::vector<VectorS<3>> samples;
                switch (static_cast<GridSamplingType>(idx)) {
                    case GridSamplingType::first : {
                        auto &grid = state->scene.get_or_emplace<sample_first_grid>(id, dims.cast<bcg_index_t>(), aabb);
                        grid = sample_first_grid(dims.cast<bcg_index_t>(), aabb);
                        grid.build(positions);
                        samples = grid.get_occupied_sample_points();
                        break;
                    }
                    case GridSamplingType::last : {
                        auto &grid = state->scene.get_or_emplace<sample_last_grid>(id, dims.cast<bcg_index_t>(), aabb);
                        grid = sample_last_grid(dims.cast<bcg_index_t>(), aabb);
                        grid.build(positions);
                        samples = grid.get_occupied_sample_points();
                        break;
                    }
                    case GridSamplingType::closest : {
                        auto &grid = state->scene.get_or_emplace<sample_closest_grid>(id, dims.cast<bcg_index_t>(),
                                                                                      aabb);
                        grid = sample_closest_grid(dims.cast<bcg_index_t>(), aabb);
                        grid.build(positions);
                        samples = grid.get_occupied_sample_points();
                        break;
                    }
                    case GridSamplingType::mean : {
                        auto &grid = state->scene.get_or_emplace<sample_mean_grid>(id, dims.cast<bcg_index_t>(), aabb);
                        grid = sample_mean_grid(dims.cast<bcg_index_t>(), aabb);
                        grid.build(positions);
                        samples = grid.get_occupied_sample_points();
                        break;
                    }
                    case GridSamplingType::__last__ : {
                        break;
                    }
                }

                auto &hierarchy = state->scene.get_or_emplace<entity_hierarchy>(state->picker.entity_id);
                entt::entity child_id = entt::null;
                for (const auto &child : hierarchy.children) {
                    if (state->scene.has<entt::tag<"subsampled_grid"_hs>>(child.first)) {
                        child_id = child.first;
                        break;
                    }
                }

                if (!samples.empty()) {
                    if (child_id == entt::null) {
                        auto parent_id = state->picker.entity_id;
                        point_cloud pc;
                        pc.positions.set(samples);
                        pc.vertices.resize(pc.positions.size());
                        pc.positions.set_dirty();
                        child_id = state->scene.create();
                        state->scene.emplace<point_cloud>(child_id, pc);
                        state->scene.emplace<entt::tag<"subsampled_grid"_hs>>(child_id);
                        state->dispatcher.trigger<event::point_cloud::setup>(child_id, "subsampled_grid");
                        state->scene.remove_if_exists<event::picking_renderer::enqueue>(child_id);
                        state->dispatcher.trigger<event::hierarchy::add_child>(parent_id, child_id);
                        state->picker.entity_id = parent_id;
                    } else {
                        auto &pc = state->scene.get<point_cloud>(child_id);
                        pc.positions.set(samples);
                        pc.vertices.resize(pc.positions.size());
                        pc.positions.set_dirty();
                    }
                }
            }
        }
    }
}

}