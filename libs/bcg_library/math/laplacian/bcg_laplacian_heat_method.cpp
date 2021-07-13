//
// Created by alex on 13.07.21.
//

#include "bcg_laplacian_heat_method.h"
#include "bcg_property_map_eigen.h"
#include "math/matrix/bcg_matrix.h"
#include "Eigen/SparseLU"
#include "mesh/bcg_mesh_face_gradient.h"
#include "mesh/bcg_mesh_vertex_divergence.h"
#include "graph/bcg_graph_edge_lengths.h"

namespace bcg{

bcg_scalar_t compute_t_squared(halfedge_mesh &mesh, bcg_scalar_t m){
    bcg_scalar_t t = 0;
    for(const auto &e : mesh.edges){
        t += edge_length(mesh, e);
    }
    t /= bcg_scalar_t(mesh.num_edges());
    return m * t * t;
}

property<bcg_scalar_t, 1> heat_method(halfedge_mesh &mesh, const laplacian_matrix &laplacian, bcg_scalar_t timestep, size_t parallel_grain_size) {
    //TODO this does not seem to be correct... figure out correct drawing of isolilnes and what is going on
    auto v_selected = mesh.vertices.template get<bool, 1>("v_selected");
    auto v_u = mesh.vertices.template get_or_add<bcg_scalar_t , 1>("v_u");
    auto v_distances = mesh.vertices.template get_or_add<bcg_scalar_t , 1>("v_geodesic_distance_heat_method");
    SparseMatrix<bcg_scalar_t> Id(v_selected.size(), v_selected.size());
    Id.setIdentity();

    SparseMatrix<bcg_scalar_t> Op = laplacian.M - timestep * laplacian.S;

    Eigen::SparseLU<SparseMatrix<bcg_scalar_t>> solver;
    solver.compute(Op);

    Map(v_u) = solver.solve(MapConst(v_selected).template cast<bcg_scalar_t>());
    auto gradients = face_gradients(mesh, v_u, parallel_grain_size);
    Map(gradients).rowwise().normalized();
    auto divergences = vertex_divergences(mesh, gradients, parallel_grain_size);

    solver.compute(laplacian.S);
    Map(v_distances) = solver.solve(MapConst(divergences));
    return v_distances;
}

}