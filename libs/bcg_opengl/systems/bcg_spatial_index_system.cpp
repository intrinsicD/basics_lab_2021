//
// Created by alex on 20.11.20.
//

#include "bcg_spatial_index_system.h"
#include "bcg_viewer_state.h"
#include "geometry/kdtree/kdtree.h"

namespace bcg{

spatial_index_system::spatial_index_system(viewer_state *state) : system("spatial_index_system", state){
    state->dispatcher.sink<event::spatial_index::setup_kdtree>().connect<&spatial_index_system::on_setup_kdtree>(this);
    state->dispatcher.sink<event::spatial_index::setup_octree>().connect<&spatial_index_system::on_setup_octree>(this);
    state->dispatcher.sink<event::spatial_index::setup_sampling_octree>().connect<&spatial_index_system::on_setup_sampling_octree>(this);
    state->dispatcher.sink<event::spatial_index::update_indices>().connect<&spatial_index_system::on_update_indices>(this);
}

void spatial_index_system::on_setup_kdtree(const event::spatial_index::setup_kdtree &event){
    if(!state->scene.valid(event.id)) return;
    if (!state->scene.has<kdtree_property<bcg_scalar_t>>(event.id)) {
        auto *vertices = state->get_vertices(event.id);
        auto positions = vertices->get<VectorS<3>, 3>("v_position");
        auto &kd_tree = state->scene.emplace<kdtree_property<bcg_scalar_t>>(event.id);
        kd_tree.build(positions);
    }
}

void spatial_index_system::on_setup_octree(const event::spatial_index::setup_octree &event){
    if(!state->scene.valid(event.id)) return;
}

void spatial_index_system::on_setup_sampling_octree(const event::spatial_index::setup_sampling_octree &event){
    if(!state->scene.valid(event.id)) return;
}

void spatial_index_system::on_update_indices(const event::spatial_index::update_indices &event){
    if(!state->scene.valid(event.id)) return;
    auto *vertices = state->get_vertices(event.id);
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    if (state->scene.has<kdtree_property<bcg_scalar_t>>(event.id)) {
        auto &kd_tree = state->scene.get<kdtree_property<bcg_scalar_t>>(event.id);
        kd_tree.build(positions);
    }
}


}