//
// Created by alex on 28.10.20.
//

#include "bcg_mesh_system.h"
#include "bcg_viewer_state.h"
#include "bcg_entity_info.h"
#include "bcg_property_map_eigen.h"
#include "geometry/aligned_box/bcg_aligned_box.h"
#include "geometry/mesh/bcg_mesh_factory.h"
#include "geometry/mesh/bcg_mesh_vertex_normals.h"
#include "geometry/mesh/bcg_mesh_face_normals.h"
#include "geometry/mesh/bcg_mesh_face_centers.h"
#include "geometry/mesh/bcg_mesh_edge_dihedral_angle.h"
#include "geometry/mesh/bcg_mesh_boundary.h"
#include "geometry/graph/bcg_graph_edge_centers.h"
#include "renderers/picking_renderer/bcg_events_picking_renderer.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"

namespace bcg {

mesh_system::mesh_system(viewer_state *state) : system("mesh_system", state) {
    state->dispatcher.sink<event::mesh::setup>().connect<&mesh_system::on_setup_mesh>(this);
    state->dispatcher.sink<event::mesh::make_triangle>().connect<&mesh_system::on_make_triangle>(this);
    state->dispatcher.sink<event::mesh::make_quad>().connect<&mesh_system::on_make_quad>(this);
    state->dispatcher.sink<event::mesh::make_box>().connect<&mesh_system::on_make_box>(this);
    state->dispatcher.sink<event::mesh::boundary>().connect<&mesh_system::on_boundary>(this);
    state->dispatcher.sink<event::mesh::vertex_normals::uniform>().connect<&mesh_system::on_vertex_normal_uniform>(
            this);
    state->dispatcher.sink<event::mesh::vertex_normals::area>().connect<&mesh_system::on_vertex_normal_area>(this);
    state->dispatcher.sink<event::mesh::vertex_normals::angle>().connect<&mesh_system::on_vertex_normal_angle>(this);
    state->dispatcher.sink<event::mesh::vertex_normals::area_angle>().connect<&mesh_system::on_vertex_normal_area_angle>(
            this);
    state->dispatcher.sink<event::mesh::face::centers>().connect<&mesh_system::on_face_centers>(this);
    state->dispatcher.sink<event::mesh::face::normals>().connect<&mesh_system::on_face_normals>(this);
    state->dispatcher.sink<event::graph::edge::centers>().connect<&mesh_system::on_edge_centers>(this);
    state->dispatcher.sink<event::mesh::edge::dihedral_angle>().connect<&mesh_system::on_dihedral_angle>(this);
}

void mesh_system::on_setup_mesh(const event::mesh::setup &event) {
    auto &mesh = state->scene.get<halfedge_mesh>(event.id);

    state->dispatcher.trigger<event::transform::add>(event.id);

    aligned_box3 aabb(mesh.positions.vector());
    state->scene.emplace<entity_info>(event.id, event.filename, "mesh", aabb.center(), aabb.halfextent().maxCoeff());

    Map(mesh.positions) =
            (MapConst(mesh.positions).rowwise() - aabb.center().transpose()) / aabb.halfextent().maxCoeff();

    state->dispatcher.trigger<event::mesh::vertex_normals::area_angle>(event.id);
    state->dispatcher.trigger<event::mesh::face::centers>(event.id);
    state->dispatcher.trigger<event::graph::edge::centers>(event.id);
    state->dispatcher.trigger<event::aligned_box::add>(event.id);
    state->scene.emplace_or_replace<event::picking_renderer::enqueue>(event.id);
    state->scene.emplace_or_replace<event::mesh_renderer::enqueue>(event.id);
    state->picker.entity_id = event.id;
    std::cout << mesh << "\n";
}

//----------------------------------------------------------------------------------------------------------------------

void mesh_system::on_make_triangle(const event::mesh::make_triangle &) {
    mesh_factory factory;

    auto mesh = factory.make_triangle();
    auto id = state->scene.create();
    state->scene.emplace<halfedge_mesh>(id, mesh);
    state->dispatcher.trigger<event::mesh::setup>(id);
}

void mesh_system::on_make_quad(const event::mesh::make_quad &) {
    mesh_factory factory;

    auto mesh = factory.make_quad();
    auto id = state->scene.create();
    state->scene.emplace<halfedge_mesh>(id, mesh);
    state->dispatcher.trigger<event::mesh::setup>(id);
}

void mesh_system::on_make_box(const event::mesh::make_box &event){
    mesh_factory factory;

    auto mesh = factory.make_box();
    auto id = state->scene.create();
    state->scene.emplace<halfedge_mesh>(id, mesh);
    state->dispatcher.trigger<event::mesh::setup>(id);
}

//----------------------------------------------------------------------------------------------------------------------

void mesh_system::on_boundary(const event::mesh::boundary &event){
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    mesh_boundary(mesh, state->config.parallel_grain_size);
}

//----------------------------------------------------------------------------------------------------------------------

void mesh_system::on_vertex_normal_uniform(const event::mesh::vertex_normals::uniform &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    vertex_normals(mesh, vertex_normal_uniform, state->config.parallel_grain_size);
}

void mesh_system::on_vertex_normal_area(const event::mesh::vertex_normals::area &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    vertex_normals(mesh, vertex_normal_area, state->config.parallel_grain_size);
}

void mesh_system::on_vertex_normal_angle(const event::mesh::vertex_normals::angle &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    vertex_normals(mesh, vertex_normal_angle, state->config.parallel_grain_size);
}

void mesh_system::on_vertex_normal_area_angle(const event::mesh::vertex_normals::area_angle &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    vertex_normals(mesh, vertex_normal_area_angle, state->config.parallel_grain_size);
}

//----------------------------------------------------------------------------------------------------------------------

void mesh_system::on_dihedral_angle(const event::mesh::edge::dihedral_angle &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    edge_dihedral_angles(mesh, state->config.parallel_grain_size);
}


void mesh_system::on_edge_centers(const event::graph::edge::centers &event){
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    edge_centers(mesh, state->config.parallel_grain_size);
}

//----------------------------------------------------------------------------------------------------------------------

void mesh_system::on_face_centers(const event::mesh::face::centers &event){
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    face_centers(mesh, state->config.parallel_grain_size);
}

void mesh_system::on_face_normals(const event::mesh::face::normals &event){
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    face_normals(mesh, state->config.parallel_grain_size);
}

//----------------------------------------------------------------------------------------------------------------------

}