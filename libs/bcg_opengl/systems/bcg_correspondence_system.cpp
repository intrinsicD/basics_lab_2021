//
// Created by alex on 04.12.20.
//

#include "bcg_correspondence_system.h"
#include "viewer/bcg_viewer_state.h"
#include "kdtree/bcg_kdtree.h"
#include "correspondences/bcg_correspondences.h"
#include "math/vector/bcg_vector_cos.h"
#include "math/vector/bcg_vector_map_eigen.h"
#include "components/bcg_component_transform_world_space.h"
#include "tbb/tbb.h"

namespace bcg {

correspondence_system::correspondence_system(viewer_state *state) : system("correspondence_system", state) {
    state->dispatcher.sink<event::correspondences::estimate>().connect<&correspondence_system::on_correspondences_estiamte>(
            this);
    state->dispatcher.sink<event::correspondences::estimate_and_filter>().connect<&correspondence_system::on_correspondences_estiamte_and_filter>(
            this);
    state->dispatcher.sink<event::correspondences::filter::distance>().connect<&correspondence_system::on_correspondences_filter_distance>(
            this);
    state->dispatcher.sink<event::correspondences::filter::normal_angle>().connect<&correspondence_system::on_correspondences_filter_normal_angle>(
            this);
}

void correspondence_system::on_correspondences_estiamte(const event::correspondences::estimate &event) {
    if (!state->scene.valid(event.source_id) || !state->scene.valid(event.target_id)) return;
    if (!state->scene.has<world_space_transform>(event.source_id)) return;
    if (!state->scene.has<world_space_transform>(event.target_id)) return;

    if (!state->scene.has<kdtree_property<bcg_scalar_t >>(event.target_id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.target_id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.target_id);

    auto &src_model = state->scene.get<world_space_transform>(event.source_id);
    auto &target_model = state->scene.get<world_space_transform>(event.target_id);
    auto src_2_target_model = target_model.inverse() * src_model;
    auto target_2_src_model = src_model.inverse() * target_model;

    vertex_container *src = state->get_vertices(event.source_id);
    vertex_container *target = state->get_vertices(event.target_id);

    auto src_positions = src->get<VectorS<3>, 3>("v_position");
    auto target_positions = target->get<VectorS<3>, 3>("v_position");

    auto corrs_idx = src->get_or_add<int, 1>("v_corrs_idx");
    auto corrs_distance = src->get_or_add<bcg_scalar_t , 1>("v_corrs_distance");
    auto corrs_vector = src->get_or_add<VectorS<3>, 3>("v_corrs_vector");
    auto corrs_valid = src->get_or_add<bool, 1>("v_corrs_valid", true);

    if(!state->scene.has<entity_correspondences>(event.source_id)){
        state->scene().emplace<entity_correspondences>(event.source_id);
    }
    auto &corrs = state->scene.get<entity_correspondences>(event.source_id);

    corrs.maps[size_t(event.target_id)].clear();
    corrs.maps[size_t(event.target_id)].mapping.resize(src->size());
    corrs.maps[size_t(event.target_id)].target_id = size_t(event.target_id);

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) src->size(), state->config.parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    VectorS<3> query_point = src_2_target_model * src_positions[i];
                    auto result = index.query_knn(query_point, 1);

                    corrs_idx[i] = result.indices[0];
                    corrs_distance[i] = result.distances[0];
                    corrs_vector[i] = target_2_src_model * target_positions[corrs_idx[i]] - src_positions[i];
                    corrs_valid[i] = true;
                    corrs.maps[size_t(event.target_id)].mapping[i] = Eigen::Triplet<bcg_scalar_t>(i, corrs_idx[i], corrs_distance[i]);

                }
            }
    );

    corrs.maps[size_t(event.target_id)].stats.clear();
    for(const auto v : *src){
        corrs.maps[size_t(event.target_id)].stats.push(corrs_distance[v]);
    }

    corrs_idx.set_dirty();
    corrs_distance.set_dirty();
    corrs_vector.set_dirty();
    corrs_valid.set_dirty();
}


void correspondence_system::on_correspondences_estiamte_and_filter(const event::correspondences::estimate_and_filter &event){
    state->dispatcher.trigger<event::correspondences::estimate>(event.source_id, event.target_id);
    if(event.filter_normal_angle){
        state->dispatcher.trigger<event::correspondences::filter::normal_angle>(event.source_id, event.target_id, event.normal_threshold);
    }
    if(event.filter_distances){
        state->dispatcher.trigger<event::correspondences::filter::distance>(event.source_id, event.target_id, event.distance_threshold);
    }
}

void correspondence_system::on_correspondences_filter_distance(const event::correspondences::filter::distance &event) {
    if (!state->scene.valid(event.source_id)) return;

    auto *src = state->get_vertices(event.source_id);

    auto corrs_distance = src->get<bcg_scalar_t, 1>("v_corrs_distance");
    auto corrs_valid = src->get<bool, 1>("v_corrs_valid");
    auto corrs_vector = src->get<VectorS<3>, 3>("v_corrs_vector");

    auto &corrs = state->scene.get<entity_correspondences>(event.source_id);
    auto &map = corrs.maps[size_t(event.target_id)];
    tbb::concurrent_vector<Eigen::Triplet<bcg_scalar_t>> mapping;
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) map.mapping.size(), state->config.parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    const auto &item = map.mapping[i];
                    if (!corrs_valid[item.row()]) continue;

                    if (corrs_distance[item.row()] > event.threshold) {
                        corrs_valid[item.row()] = false;
                        corrs_vector[item.row()].setZero();
                        corrs_distance[item.row()] = event.threshold;
                    }else{
                        mapping.push_back(item);
                    }
                }
            }
    );

    correspondences filtered_corrs;
    filtered_corrs.mapping.resize(mapping.size());
    std::copy(mapping.begin(), mapping.end(), filtered_corrs.mapping.begin());
    filtered_corrs.target_id = map.target_id;
    filtered_corrs.max_source_index = MapConst(filtered_corrs.source_indices()).maxCoeff();
    filtered_corrs.max_target_index = MapConst(filtered_corrs.target_indices()).maxCoeff();
    map = filtered_corrs;

    corrs.maps[size_t(event.target_id)].stats.clear();
    for(const auto v : *src){
        if(corrs_valid[v]){
            map.stats.push(corrs_distance[v]);
        }
    }

    corrs_valid.set_dirty();
    corrs_vector.set_dirty();
}

void correspondence_system::on_correspondences_filter_normal_angle(
        const event::correspondences::filter::normal_angle &event) {
    if (!state->scene.valid(event.source_id)) return;
    if (!state->scene.valid(event.target_id)) return;

    if (!state->scene.has<world_space_transform>(event.source_id)) return;
    if (!state->scene.has<world_space_transform>(event.target_id)) return;

    auto &src_model = state->scene.get<world_space_transform>(event.source_id);
    auto &target_model = state->scene.get<world_space_transform>(event.target_id);

    auto *src = state->get_vertices(event.source_id);
    auto *target = state->get_vertices(event.target_id);

    auto src_2_target = target_model.inverse() * src_model;

    auto src_normals = src->get<VectorS<3>, 3>("v_normal");
    auto target_normals = target->get<VectorS<3>, 3>("v_normal");

    auto corrs_valid = src->get<bool, 1>("v_corrs_valid");
    auto corrs_vector = src->get<VectorS<3>, 3>("v_corrs_vector");

    auto &corrs = state->scene.get<entity_correspondences>(event.source_id);
    auto &map = corrs.maps[size_t(event.target_id)];

    tbb::concurrent_vector<Eigen::Triplet<bcg_scalar_t>> mapping;
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) map.mapping.size(), state->config.parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    const auto &item = map.mapping[i];
                    if (!corrs_valid[item.row()]) continue;

                    VectorS<3> src_normal = src_2_target.linear() * src_normals[item.row()];
                    bcg_scalar_t cos = clamp_cos(vector_cos(src_normal, target_normals[item.col()]));

                    if (cos < 0 || std::acos(cos) > event.threshold) {
                        corrs_valid[item.row()] = false;
                        corrs_vector[item.row()].setZero();
                    }else{
                        mapping.push_back(item);
                    }
                }
            }
    );

    correspondences filtered_corrs;
    filtered_corrs.mapping.resize(mapping.size());
    std::copy(mapping.begin(), mapping.end(), filtered_corrs.mapping.begin());
    filtered_corrs.target_id = map.target_id;
    filtered_corrs.max_source_index = MapConst(filtered_corrs.source_indices()).maxCoeff();
    filtered_corrs.max_target_index = MapConst(filtered_corrs.target_indices()).maxCoeff();
    map = filtered_corrs;


    auto corrs_distance = src->get<bcg_scalar_t, 1>("v_corrs_distance");
    corrs.maps[size_t(event.target_id)].stats.clear();
    for(const auto v : *src){
        if(corrs_valid[v]){
            map.stats.push(corrs_distance[v]);
        }
    }

    corrs_valid.set_dirty();
    corrs_vector.set_dirty();
}

}