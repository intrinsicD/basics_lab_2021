//
// Created by alex on 19.11.20.
//

#include "bcg_mesh_features.h"
#include "bcg_mesh_boundary.h"
#include "bcg_mesh_edge_dihedral_angle.h"
#include "tbb/tbb.h"

namespace bcg {

void mesh_features_boundary(halfedge_mesh &mesh, size_t parallel_grain_size){
    auto feature_vertices = mesh.vertices.get_or_add<bool, 1>("v_feature");
    auto feature_edges = mesh.edges.get_or_add<bool, 1>("e_feature");

    if(!mesh.vertices.has("v_boundary") || !mesh.edges.has("e_boundary")){
        mesh_boundary(mesh, parallel_grain_size);
    }

    if(!mesh.vertices.has("e_dihedral_angle")){
        edge_dihedral_angles(mesh, parallel_grain_size);
    }
    auto v_boundary = mesh.vertices.get<bool, 1>("v_boundary");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    feature_vertices[v] = feature_vertices[v] || v_boundary[v];
                }
            }
    );
    auto e_boundary = mesh.edges.get<bool, 1>("e_boundary");
    auto e_dihedral_angle = mesh.edges.get<bcg_scalar_t, 1>("e_dihedral_angle");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);

                    feature_edges[e] = feature_edges[e] || e_boundary[e];

                }
            }
    );

    feature_vertices.set_dirty();
    feature_edges.set_dirty();
}

void mesh_features_dihedral_angle(halfedge_mesh &mesh, bcg_scalar_t threshold_degrees, size_t parallel_grain_size){
    auto feature_vertices = mesh.vertices.get_or_add<bool, 1>("v_feature");
    auto feature_edges = mesh.edges.get_or_add<bool, 1>("e_feature");

    if(!mesh.vertices.has("e_dihedral_angle")){
        edge_dihedral_angles(mesh, parallel_grain_size);
    }

    auto e_dihedral_angle = mesh.edges.get<bcg_scalar_t, 1>("e_dihedral_angle");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);

                    feature_edges[e] = feature_edges[e]  || (e_dihedral_angle[e] > threshold_degrees);

                }
            }
    );

    feature_vertices.set_dirty();
    feature_edges.set_dirty();
}

void mesh_features(halfedge_mesh &mesh, bool boundary, bool angle, bcg_scalar_t threshold_degrees, size_t parallel_grain_size){
    if(boundary) mesh_features_boundary(mesh, parallel_grain_size);
    if(angle) mesh_features_dihedral_angle(mesh, threshold_degrees, parallel_grain_size);
}

void mesh_clear_features(halfedge_mesh &mesh){
    mesh.vertices.remove("v_feature");
    mesh.edges.remove("e_feature");
}

}