//
// Created by alex on 28.10.20.
//

#include "bcg_graph_system.h"
#include "viewer/bcg_viewer_state.h"
#include "aligned_box/bcg_aligned_box.h"
#include "components/bcg_component_entity_info.h"
#include "components/bcg_component_loading_backup.h"
#include "components/bcg_component_transform_world_space.h"
#include "components/bcg_component_transform_object_space.h"
#include "bcg_property_map_eigen.h"
#include "renderers/picking_renderer/bcg_events_picking_renderer.h"
#include "renderers/graph_renderer/bcg_events_graph_renderer.h"
#include "graph/bcg_graph_vertex_pca.h"

namespace bcg{

graph_system::graph_system(viewer_state *state) : system("graph_system", state){
    state->dispatcher.sink<event::graph::setup>().connect<&graph_system::on_setup>(this);
    state->dispatcher.sink<event::graph::vertex::pca::svd>().connect<&graph_system::on_vertex_pca_svd>(this);
    state->dispatcher.sink<event::graph::vertex::pca::weighted_svd>().connect<&graph_system::on_vertex_pca_weighted_svd>(this);
    state->dispatcher.sink<event::graph::vertex::pca::eig>().connect<&graph_system::on_vertex_pca_eig>(this);
    state->dispatcher.sink<event::graph::vertex::pca::weighted_eig>().connect<&graph_system::on_vertex_pca_weighted_eig>(this);
}

void graph_system::on_setup(const event::graph::setup &event){
    auto &graph = state->scene.get<halfedge_graph>(event.id);

    auto &backup = state->scene().emplace<loading_backup>(event.id);
    backup.aabb = aligned_box3(graph.positions.vector());
    bcg_scalar_t scale = backup.aabb.halfextent().maxCoeff();
    backup.os_model.linear() = Scaling(scale, scale, scale);
    backup.os_model.translation() = backup.aabb.center();

    state->dispatcher.trigger<event::transform::world_space::init>(event.id);
    state->dispatcher.trigger<event::transform::object_space::init>(event.id);
    state->dispatcher.trigger<event::aligned_box::set>(event.id, backup.aabb);

    state->scene().emplace<entity_info>(event.id, event.filename, "graph");

    state->dispatcher.trigger<event::mesh::vertex_normals::area_angle>(event.id);
    state->dispatcher.trigger<event::mesh::face::centers>(event.id);
    state->dispatcher.trigger<event::graph::edge::centers>(event.id);
    state->dispatcher.trigger<event::aligned_box::add>(event.id);
    state->scene().emplace_or_replace<event::picking_renderer::enqueue>(event.id);
    state->scene().emplace_or_replace<event::graph_renderer::enqueue>(event.id);
    state->picker.entity_id = event.id;
    std::cout << graph << "\n";
}

void graph_system::on_vertex_pca_svd(const event::graph::vertex::pca::svd &event){
    if(!state->scene.valid(event.id)) return;
    if(!state->scene.has<halfedge_graph>(event.id)) return;
    halfedge_graph &graph = state->scene.get<halfedge_graph>(event.id);

    graph_vertex_pcas(graph, graph_vertex_pca_least_squares_svd, event.compute_mean,
                      state->config.parallel_grain_size);
}

void graph_system::on_vertex_pca_weighted_svd(const event::graph::vertex::pca::weighted_svd &event){
    if(!state->scene.valid(event.id)) return;
    if(!state->scene.has<halfedge_graph>(event.id)) return;
    halfedge_graph &graph = state->scene.get<halfedge_graph>(event.id);

    graph_vertex_pcas(graph, graph_vertex_pca_weighted_least_squares_svd, event.compute_mean,
                      state->config.parallel_grain_size);
}

void graph_system::on_vertex_pca_eig(const event::graph::vertex::pca::eig &event){
    if(!state->scene.valid(event.id)) return;
    if(!state->scene.has<halfedge_graph>(event.id)) return;
    halfedge_graph &graph = state->scene.get<halfedge_graph>(event.id);

    graph_vertex_pcas(graph, graph_vertex_pca_least_squares_eig, event.compute_mean,
                      state->config.parallel_grain_size);
}

void graph_system::on_vertex_pca_weighted_eig(const event::graph::vertex::pca::weighted_eig &event){
    if(!state->scene.valid(event.id)) return;
    if(!state->scene.has<halfedge_graph>(event.id)) return;
    halfedge_graph &graph = state->scene.get<halfedge_graph>(event.id);

    graph_vertex_pcas(graph, graph_vertex_pca_weighted_least_squares_eig, event.compute_mean,
                      state->config.parallel_grain_size);
}

}