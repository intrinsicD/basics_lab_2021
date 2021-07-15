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
#include "igl/heat_geodesics.h"

namespace bcg {

bcg_scalar_t compute_t(halfedge_mesh &mesh) {
    bcg_scalar_t t = 0;
    for (const auto &e : mesh.edges) {
        t += edge_length(mesh, e);
    }
    t /= bcg_scalar_t(mesh.num_edges());
    return t;
}

property<bcg_scalar_t, 1>
heat_method(halfedge_mesh &mesh, const laplacian_matrix &laplacian, bcg_scalar_t timestep, size_t parallel_grain_size) {
    //TODO this does not seem to be correct... figure out correct drawing of isolilnes and what is going on
    auto v_selected = mesh.vertices.template get<bool, 1>("v_selected");
    auto v_gt_distances = mesh.vertices.template get_or_add<bcg_scalar_t, 1>("v_gt_distance");
    auto v_u = mesh.vertices.template get_or_add<bcg_scalar_t, 1>("v_u");
    auto v_distances = mesh.vertices.template get_or_add<bcg_scalar_t, 1>("v_geodesic_distance_heat_method");
    /*SparseMatrix<bcg_scalar_t> Id(v_selected.size(), v_selected.size());
    Id.setIdentity();

    SparseMatrix<bcg_scalar_t> Op = laplacian.M - timestep * laplacian.S;

    Eigen::SparseLU<SparseMatrix<bcg_scalar_t>> solver;
    solver.compute(Op);

    Map(v_u) = solver.solve(MapConst(v_selected).template cast<bcg_scalar_t>());
    Map(v_u) = MapConst(v_u).cwiseAbs();
    auto gradients = face_gradients(mesh, v_u, parallel_grain_size);
    Map(gradients).rowwise().normalized();
    Map(gradients) = -MapConst(gradients);
    auto divergences = vertex_divergences(mesh, gradients, parallel_grain_size);
    //Map(divergences).array() -= MapConst(divergences).mean();
    solver.compute(laplacian.S);
    Map(v_distances) = solver.solve(MapConst(divergences));
    Map(v_distances).array() -= MapConst(v_distances).minCoeff();*/

    igl::HeatGeodesicsData<double> data;
    MatrixS<-1, 3> P = MapConst(mesh.positions);
    Matrix<int,-1, 3> F = MapConst(mesh.get_triangles()).cast<int>();
    igl::heat_geodesics_precompute(P, F, data);

    MatrixS<-1, 1> D(mesh.num_vertices(), 1);

    Matrix<unsigned int, -1, 1> G(MapConst(v_selected).sum(), 1);
    for(const auto v : mesh.vertices){
        if(v_selected[v]){
            G << v.idx;
        }
    }
    igl::heat_geodesics_solve(data, G, D);
    Map(v_distances) = D;
    for (const auto v : mesh.vertices) {
        if (v_selected[v]) {
            for (const auto vv : mesh.halfedge_graph::get_vertices(v)) {
                v_gt_distances[vv] = (mesh.positions[v] - mesh.positions[vv]).norm();
            }
        }
    }
    v_gt_distances.set_dirty();
    v_distances.set_dirty();
/*    gradients.set_dirty();
    divergences.set_dirty();*/
    return v_distances;
}

}