//
// Created by alex on 28.10.20.
//

#include "bcg_point_cloud_system.h"
#include "bcg_viewer_state.h"
#include "aligned_box/bcg_aligned_box.h"
#include "bcg_entity_info.h"
#include "bcg_property_map_eigen.h"
#include "renderers/picking_renderer/bcg_events_picking_renderer.h"
#include "renderers/points_renderer/bcg_events_points_renderer.h"
#include "point_cloud/bcg_point_cloud_graph_builder.h"

namespace bcg {

point_cloud_system::point_cloud_system(viewer_state *state) : system("point_cloud_system", state) {
    state->dispatcher.sink<event::point_cloud::setup>().connect<&point_cloud_system::on_setup>(this);
    state->dispatcher.sink<event::point_cloud::build_graph>().connect<&point_cloud_system::on_build_graph>(this);
}

void point_cloud_system::on_setup(const event::point_cloud::setup &event) {
    auto &pc = state->scene.get<point_cloud>(event.id);

    state->dispatcher.trigger<event::transform::add>(event.id);

    aligned_box3 aabb(pc.positions.vector());
    state->scene.emplace<entity_info>(event.id, event.filename, "point_cloud", aabb.center(),
                                      aabb.halfextent().maxCoeff());

    Map(pc.positions) =
            (MapConst(pc.positions).rowwise() - aabb.center().transpose()) / aabb.halfextent().maxCoeff();

    state->dispatcher.trigger<event::aligned_box::add>(event.id);
    state->scene.emplace_or_replace<event::picking_renderer::enqueue>(event.id);
    state->scene.emplace_or_replace<event::points_renderer::enqueue>(event.id);
    state->picker.entity_id = event.id;
    std::cout << pc << "\n";
}

void point_cloud_system::on_build_graph(const event::point_cloud::build_graph &event) {
    if (!state->scene.valid(event.id)) return;
    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;

    switch (event.type) {
        case GraphType::kdtree_knn: {
            if (!state->scene.has<kdtree_property<bcg_scalar_t >>(event.id)) {
                state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
            }
            auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);
            auto graph = point_cloud_build_knn_graph(*vertices, index, event.num_closest);
            if(graph.num_edges() != 0){
                auto id = state->scene.create();
                state->scene.emplace<halfedge_graph>(id, graph);
                state->dispatcher.trigger<event::graph::setup>(id);
            }
            break;
        }
        case GraphType::kdtree_radius: {
            if (!state->scene.has<kdtree_property<bcg_scalar_t >>(event.id)) {
                state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
            }
            auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);
            auto graph = point_cloud_build_radius_graph(*vertices, index, event.radius);
            if(graph.num_edges() != 0){
                auto id = state->scene.create();
                state->scene.emplace<halfedge_graph>(id, graph);
                state->dispatcher.trigger<event::graph::setup>(id);
            }
            break;
        }
        case GraphType::octree_knn: {
            if (!state->scene.has<kdtree_property<bcg_scalar_t >>(event.id)) {
                state->dispatcher.trigger<event::spatial_index::setup_octree>(event.id);
            }
            auto &index = state->scene.get<octree>(event.id);
            auto graph = point_cloud_build_knn_graph(*vertices, index, event.num_closest);
            if(graph.num_edges() != 0){
                auto id = state->scene.create();
                state->scene.emplace<halfedge_graph>(id, graph);
                state->dispatcher.trigger<event::graph::setup>(id);
            }
            break;
        }
        case GraphType::octree_radius: {
            if (!state->scene.has<kdtree_property<bcg_scalar_t >>(event.id)) {
                state->dispatcher.trigger<event::spatial_index::setup_octree>(event.id);
            }
            auto &index = state->scene.get<octree>(event.id);
            auto graph = point_cloud_build_radius_graph(*vertices, index, event.radius);
            if(graph.num_edges() != 0){
                auto id = state->scene.create();
                state->scene.emplace<halfedge_graph>(id, graph);
                state->dispatcher.trigger<event::graph::setup>(id);
            }
            break;
        }
        case GraphType::__last__: {

        }
        default: {

        }
    }
}
}