//
// Created by alex on 28.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_SYSTEM_H
#define BCG_GRAPHICS_BCG_MESH_SYSTEM_H

#include "bcg_systems.h"

namespace bcg {

struct mesh_system : public system {
    explicit mesh_system(viewer_state *state);

    void on_setup(const event::mesh::setup &event);

    //------------------------------------------------------------------------------------------------------------------

    void on_make_triangle(const event::mesh::make_triangle &event);

    void on_make_quad(const event::mesh::make_quad &event);

    void on_make_box(const event::mesh::make_box &event);

    //------------------------------------------------------------------------------------------------------------------

    void on_boundary(const event::mesh::boundary &event);

    void on_vertex_convex_concave(const event::mesh::vertex_convex_concave &event);

    void on_features(const event::mesh::features &event);

    void on_features_clear(const event::mesh::features_clear &event);

    void on_subdivision_catmull_clark(const event::mesh::subdivision::catmull_clark &event);

    void on_subdivision_loop(const event::mesh::subdivision::loop &event);

    void on_subdivision_sqrt3(const event::mesh::subdivision::sqrt3 &event);

    void on_connected_components_detect(const event::mesh::connected_components::detect &event);

    void on_connected_components_split(const event::mesh::connected_components::split &event);

    void on_build_laplacian(const event::mesh::laplacian::build &event);

    void on_curvature_taubin(const event::mesh::curvature::taubin &event);

    void on_simplification(const event::mesh::simplification &event);

    void on_remeshing_uniform(const event::mesh::remeshing::uniform &event);

    void on_remeshing_adaptive(const event::mesh::remeshing::adaptive &event);

    void on_statistics(const event::mesh::statistics &event);

    void on_smoothing_explicit(const event::mesh::smoothing::explicit_smoothing &event);
    void on_smoothing_implicit(const event::mesh::smoothing::implicit_smoothing &event);
    void on_smoothing_explicit_1D(const event::mesh::smoothing::explicit_smoothing_1D &event);
    void on_smoothing_implicit_1D(const event::mesh::smoothing::implicit_smoothing_1D &event);
    void on_smoothing_explicit_3D(const event::mesh::smoothing::explicit_smoothing_3D &event);
    void on_smoothing_implicit_3D(const event::mesh::smoothing::implicit_smoothing_3D &event);

    //------------------------------------------------------------------------------------------------------------------

    void on_vertex_normal_uniform(const event::mesh::vertex_normals::uniform &event);

    void on_vertex_normal_area(const event::mesh::vertex_normals::area &event);

    void on_vertex_normal_angle(const event::mesh::vertex_normals::angle &event);

    void on_vertex_normal_area_angle(const event::mesh::vertex_normals::area_angle &event);

    //------------------------------------------------------------------------------------------------------------------

    void on_dihedral_angle(const event::mesh::edge::dihedral_angle &event);

    void on_edge_centers(const event::graph::edge::centers &event);

    //------------------------------------------------------------------------------------------------------------------

    void on_face_centers(const event::mesh::face::centers &event);

    void on_face_normals(const event::mesh::face::normals &event);

    //------------------------------------------------------------------------------------------------------------------
};

}

#endif //BCG_GRAPHICS_BCG_MESH_SYSTEM_H
