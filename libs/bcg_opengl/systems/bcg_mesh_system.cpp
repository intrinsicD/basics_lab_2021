//
// Created by alex on 28.10.20.
//

#include "bcg_mesh_system.h"
#include "viewer/bcg_viewer_state.h"
#include "components/bcg_component_entity_info.h"
#include "components/bcg_component_transform_world_space.h"
#include "components/bcg_component_transform_object_space.h"
#include "components/bcg_component_loading_backup.h"
#include "bcg_property_map_eigen.h"
#include "geometry/aligned_box/bcg_aligned_box.h"
#include "geometry/mesh/bcg_mesh_factory.h"
#include "geometry/mesh/bcg_mesh_vertex_normals.h"
#include "geometry/mesh/bcg_mesh_face_normals.h"
#include "geometry/mesh/bcg_mesh_face_centers.h"
#include "geometry/mesh/bcg_mesh_edge_dihedral_angle.h"
#include "geometry/mesh/bcg_mesh_boundary.h"
#include "geometry/mesh/bcg_mesh_vertex_convex_concave.h"
#include "geometry/mesh/bcg_mesh_features.h"
#include "geometry/mesh/bcg_mesh_subdivision.h"
#include "geometry/mesh/bcg_mesh_connected_components.h"
#include "geometry/mesh/bcg_mesh_laplacian.h"
#include "geometry/mesh/bcg_mesh_curvature_taubin.h"
#include "geometry/mesh/bcg_mesh_simplification.h"
#include "geometry/mesh/bcg_mesh_remeshing.h"
#include "geometry/mesh/bcg_mesh_statistics.h"
#include "geometry/mesh/bcg_mesh_smoothing.h"
#include "geometry/graph/bcg_graph_edge_centers.h"
#include "renderers/picking_renderer/bcg_events_picking_renderer.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "utils/bcg_path.h"

namespace bcg {

using namespace entt::literals;

mesh_system::mesh_system(viewer_state *state) : system("mesh_system", state) {
    state->dispatcher.sink<event::mesh::setup>().connect<&mesh_system::on_setup>(this);
    state->dispatcher.sink<event::mesh::make_triangle>().connect<&mesh_system::on_make_triangle>(this);
    state->dispatcher.sink<event::mesh::make_quad>().connect<&mesh_system::on_make_quad>(this);
    state->dispatcher.sink<event::mesh::make_box>().connect<&mesh_system::on_make_box>(this);
    state->dispatcher.sink<event::mesh::make_parameterized_plane>().connect<&mesh_system::on_make_parameterized_plane>(
            this);
    state->dispatcher.sink<event::mesh::boundary>().connect<&mesh_system::on_boundary>(this);
    state->dispatcher.sink<event::mesh::vertex_convex_concave>().connect<&mesh_system::on_vertex_convex_concave>(this);
    state->dispatcher.sink<event::mesh::features>().connect<&mesh_system::on_features>(this);
    state->dispatcher.sink<event::mesh::features_clear>().connect<&mesh_system::on_features_clear>(this);
    state->dispatcher.sink<event::mesh::subdivision::catmull_clark>().connect<&mesh_system::on_subdivision_catmull_clark>(
            this);
    state->dispatcher.sink<event::mesh::subdivision::loop>().connect<&mesh_system::on_subdivision_loop>(this);
    state->dispatcher.sink<event::mesh::subdivision::sqrt3>().connect<&mesh_system::on_subdivision_sqrt3>(this);
    state->dispatcher.sink<event::mesh::connected_components::detect>().connect<&mesh_system::on_connected_components_detect>(
            this);
    state->dispatcher.sink<event::mesh::connected_components::split>().connect<&mesh_system::on_connected_components_split>(
            this);
    state->dispatcher.sink<event::mesh::laplacian::build>().connect<&mesh_system::on_build_laplacian>(this);
    state->dispatcher.sink<event::mesh::curvature::taubin>().connect<&mesh_system::on_curvature_taubin>(this);
    state->dispatcher.sink<event::mesh::simplification>().connect<&mesh_system::on_simplification>(this);
    state->dispatcher.sink<event::mesh::remeshing::uniform>().connect<&mesh_system::on_remeshing_uniform>(this);
    state->dispatcher.sink<event::mesh::remeshing::adaptive>().connect<&mesh_system::on_remeshing_adaptive>(this);
    state->dispatcher.sink<event::mesh::statistics>().connect<&mesh_system::on_statistics>(this);
    state->dispatcher.sink<event::mesh::smoothing::explicit_smoothing>().connect<&mesh_system::on_smoothing_explicit>(
            this);
    state->dispatcher.sink<event::mesh::smoothing::explicit_smoothing_1D>().connect<&mesh_system::on_smoothing_explicit_1D>(
            this);
    state->dispatcher.sink<event::mesh::smoothing::explicit_smoothing_3D>().connect<&mesh_system::on_smoothing_explicit_3D>(
            this);
    state->dispatcher.sink<event::mesh::smoothing::implicit_smoothing>().connect<&mesh_system::on_smoothing_implicit>(
            this);
    state->dispatcher.sink<event::mesh::smoothing::implicit_smoothing_1D>().connect<&mesh_system::on_smoothing_implicit_1D>(
            this);
    state->dispatcher.sink<event::mesh::smoothing::implicit_smoothing_3D>().connect<&mesh_system::on_smoothing_implicit_3D>(
            this);
    state->dispatcher.sink<event::mesh::smoothing::taubin_smoothing>().connect<&mesh_system::on_smoothing_taubin>(this);
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

void mesh_system::on_setup(const event::mesh::setup &event) {
    auto &mesh = state->scene.get<halfedge_mesh>(event.id);

    auto &backup = state->scene().emplace<loading_backup>(event.id);
    backup.aabb = aligned_box3(mesh.positions.vector());
    bcg_scalar_t scale = backup.aabb.halfextent().maxCoeff();
    backup.os_model.linear() = Scaling(scale, scale, scale);
    backup.os_model.translation() = backup.aabb.center();

    state->dispatcher.trigger<event::transform::world_space::init>(event.id);
    state->dispatcher.trigger<event::transform::object_space::init>(event.id);
    state->dispatcher.trigger<event::aligned_box::set>(event.id, backup.aabb);

    state->scene().emplace<entity_info>(event.id, event.filename, "mesh");

    if (!mesh.vertices.has("v_normal")) {
        state->dispatcher.trigger<event::mesh::vertex_normals::area_angle>(event.id);
    }
    state->dispatcher.trigger<event::mesh::face::centers>(event.id);
    state->dispatcher.trigger<event::graph::edge::centers>(event.id);
    state->scene().emplace_or_replace<event::picking_renderer::enqueue>(event.id);
    state->scene().emplace_or_replace<event::mesh_renderer::enqueue>(event.id);
    state->picker.entity_id = event.id;
    std::cout << mesh << "\n";
}

//----------------------------------------------------------------------------------------------------------------------

void mesh_system::on_make_triangle(const event::mesh::make_triangle &) {
    mesh_factory factory;

    auto mesh = factory.make_triangle();
    auto id = state->scene.create();
    state->scene().emplace<halfedge_mesh>(id, mesh);
    state->dispatcher.trigger<event::mesh::setup>(id, "");
}

void mesh_system::on_make_quad(const event::mesh::make_quad &) {
    mesh_factory factory;

    auto mesh = factory.make_quad();
    auto id = state->scene.create();
    state->scene().emplace<halfedge_mesh>(id, mesh);
    state->dispatcher.trigger<event::mesh::setup>(id, "");
}

void mesh_system::on_make_box(const event::mesh::make_box &) {
    mesh_factory factory;

    auto mesh = factory.make_box();
    auto id = state->scene.create();
    state->scene().emplace<halfedge_mesh>(id, mesh);
    state->dispatcher.trigger<event::mesh::setup>(id, "");
}

void mesh_system::on_make_parameterized_plane(const event::mesh::make_parameterized_plane &) {
    mesh_factory factory;

    ParameterizedPlane3 plane;
    auto mesh = factory.make_parameterized_plane(plane);
    auto id = state->scene.create();
    state->scene().emplace<halfedge_mesh>(id, mesh);
    state->scene().emplace<ParameterizedPlane3>(id, plane);
    state->scene().emplace<entt::tag<"parameterized_plane"_hs>>(id);
    assert(state->scene.has<entt::tag<"parameterized_plane"_hs>>(id));
    state->dispatcher.trigger<event::mesh::setup>(id, "");
}

//----------------------------------------------------------------------------------------------------------------------

void mesh_system::on_boundary(const event::mesh::boundary &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    mesh_boundary(mesh, state->config.parallel_grain_size);
}

void mesh_system::on_vertex_convex_concave(const event::mesh::vertex_convex_concave &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    vertex_convex_concave(mesh, event.post_smoothing_steps, event.two_ring_neighborhood,
                          state->config.parallel_grain_size);
}

void mesh_system::on_features(const event::mesh::features &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    mesh_features(mesh, event.boundary, event.angle, event.threshold_degrees, state->config.parallel_grain_size);
}

void mesh_system::on_features_clear(const event::mesh::features_clear &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    mesh_clear_features(mesh);
}

void mesh_system::on_subdivision_catmull_clark(const event::mesh::subdivision::catmull_clark &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    mesh_subdivision_catmull_clark(mesh, state->config.parallel_grain_size);
    state->dispatcher.trigger<event::mesh::vertex_normals::area_angle>(event.id);
    state->dispatcher.trigger<event::spatial_index::update_indices>(event.id);
}

void mesh_system::on_subdivision_loop(const event::mesh::subdivision::loop &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    mesh_subdivision_loop(mesh, state->config.parallel_grain_size);
    state->dispatcher.trigger<event::mesh::vertex_normals::area_angle>(event.id);
    state->dispatcher.trigger<event::spatial_index::update_indices>(event.id);
}

void mesh_system::on_subdivision_sqrt3(const event::mesh::subdivision::sqrt3 &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    mesh_subdivision_sqrt3(mesh, state->config.parallel_grain_size);
    state->dispatcher.trigger<event::mesh::vertex_normals::area_angle>(event.id);
    state->dispatcher.trigger<event::spatial_index::update_indices>(event.id);
}

void mesh_system::on_connected_components_detect(const event::mesh::connected_components::detect &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    mesh_connected_components_detect(mesh);
}

void mesh_system::on_connected_components_split(const event::mesh::connected_components::split &event) {
    auto parent_id = event.id;
    if (!state->scene.valid(parent_id)) return;
    if (!state->scene.has<halfedge_mesh>(parent_id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(parent_id);
    auto info = state->scene.get<entity_info>(parent_id);
    auto parent_model = state->scene.get<world_space_transform>(parent_id);
    auto parts = mesh_connected_components_split(mesh);
    for (const auto &part : parts) {
        auto child_id = state->scene.create();
        state->scene().emplace<halfedge_mesh>(child_id, part);
        std::string filename = path_join(path_dirname(info.filename), path_basename(info.filename)) + "_part_" +
                               std::to_string(int(child_id)) + path_extension(info.filename);
        state->dispatcher.trigger<event::mesh::setup>(child_id, filename);
        state->dispatcher.trigger<event::transform::world_space::set>(child_id, parent_model);
        //state->dispatcher.trigger<event::hierarchy::add_child>(event.id, id);
        //state->dispatcher.trigger<event::hierarchy::set_parent>(id, event.id);
    }
}

void mesh_system::on_build_laplacian(const event::mesh::laplacian::build &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    auto laplacian = build_laplacian(mesh, event.s_type, event.m_type, state->config.parallel_grain_size,
                                     event.edge_scaling_property_name);
    state->scene().emplace_or_replace<mesh_laplacian>(event.id, laplacian);
}

void mesh_system::on_curvature_taubin(const event::mesh::curvature::taubin &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    mesh_curvature_taubin(mesh, event.post_smoothing_steps, event.two_ring_neighborhood,
                          state->config.parallel_grain_size);
}

void mesh_system::on_simplification(const event::mesh::simplification &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    mesh_simplification(mesh, event.n_vertices, event.aspect_ratio,
                        event.edge_length,
                        event.max_valence,
                        event.normal_deviation,
                        event.hausdorff_error);
    state->dispatcher.trigger<event::mesh::vertex_normals::area_angle>(event.id);
    state->dispatcher.trigger<event::spatial_index::update_indices>(event.id);
}

void mesh_system::on_remeshing_uniform(const event::mesh::remeshing::uniform &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    mesh_remeshing_uniform(mesh, event.edge_length, event.iterations, event.use_projection);

    state->dispatcher.trigger<event::mesh::vertex_normals::area_angle>(event.id);
    state->dispatcher.trigger<event::spatial_index::update_indices>(event.id);
}

void mesh_system::on_remeshing_adaptive(const event::mesh::remeshing::adaptive &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    mesh_remeshing_adaptive(mesh, event.min_edge_length, event.max_edge_length, event.approx_error, event.iterations,
                            event.use_projection);

    state->dispatcher.trigger<event::mesh::vertex_normals::area_angle>(event.id);
    state->dispatcher.trigger<event::spatial_index::update_indices>(event.id);
}

void mesh_system::on_statistics(const event::mesh::statistics &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    auto stats = mesh_statistics(mesh, state->config.parallel_grain_size);
    state->scene().emplace_or_replace<mesh_stats>(event.id, stats);
}

void mesh_system::on_smoothing_explicit(const event::mesh::smoothing::explicit_smoothing &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;
    if (!state->scene.has<mesh_laplacian>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    auto &laplacian = state->scene.get<mesh_laplacian>(event.id);
    explicit_smoothing(mesh, laplacian, event.smoothing_steps, event.timestep, state->config.parallel_grain_size);
    state->dispatcher.trigger<event::mesh::vertex_normals::area_angle>(event.id);
}

void mesh_system::on_smoothing_implicit(const event::mesh::smoothing::implicit_smoothing &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    auto &laplacian = state->scene.get<mesh_laplacian>(event.id);
    implicit_smoothing(mesh, laplacian, event.timestep);
    state->dispatcher.trigger<event::mesh::vertex_normals::area_angle>(event.id);
}

void mesh_system::on_smoothing_explicit_1D(const event::mesh::smoothing::explicit_smoothing_1D &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    auto &laplacian = state->scene.get<mesh_laplacian>(event.id);
    auto p = event.prop;
    explicit_smoothing(mesh, laplacian, p, event.smoothing_steps, event.timestep, state->config.parallel_grain_size);
}

void mesh_system::on_smoothing_implicit_1D(const event::mesh::smoothing::implicit_smoothing_1D &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    auto &laplacian = state->scene.get<mesh_laplacian>(event.id);
    auto p = event.prop;
    implicit_smoothing<bcg_scalar_t, 1>(mesh, laplacian, p, event.timestep);
}

void mesh_system::on_smoothing_explicit_3D(const event::mesh::smoothing::explicit_smoothing_3D &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    auto &laplacian = state->scene.get<mesh_laplacian>(event.id);
    auto p = event.prop;
    explicit_smoothing(mesh, laplacian, p, event.smoothing_steps, event.timestep, state->config.parallel_grain_size);
}

void mesh_system::on_smoothing_implicit_3D(const event::mesh::smoothing::implicit_smoothing_3D &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    auto &laplacian = state->scene.get<mesh_laplacian>(event.id);
    auto p = event.prop;
    implicit_smoothing(mesh, laplacian, p, event.timestep);
}

void mesh_system::on_smoothing_taubin(const event::mesh::smoothing::taubin_smoothing &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    auto &laplacian = state->scene.get<mesh_laplacian>(event.id);
    taubin_smoothing(mesh, laplacian, event.lambda, event.mu, event.smoothing_steps);
    state->dispatcher.trigger<event::mesh::vertex_normals::area_angle>(event.id);
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


void mesh_system::on_edge_centers(const event::graph::edge::centers &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    edge_centers(mesh, state->config.parallel_grain_size);
}

//----------------------------------------------------------------------------------------------------------------------

void mesh_system::on_face_centers(const event::mesh::face::centers &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    face_centers(mesh, state->config.parallel_grain_size);
}

void mesh_system::on_face_normals(const event::mesh::face::normals &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;

    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
    face_normals(mesh, state->config.parallel_grain_size);
}

//----------------------------------------------------------------------------------------------------------------------

}