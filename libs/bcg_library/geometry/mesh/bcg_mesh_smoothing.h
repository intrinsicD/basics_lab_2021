//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_SMOOTHING_H
#define BCG_GRAPHICS_BCG_MESH_SMOOTHING_H

#include "bcg_mesh.h"
#include "bcg_mesh_laplacian.h"
#include "bcg_property_map_eigen.h"
#include "Eigen/SparseCholesky"

namespace bcg {

void rescale(halfedge_mesh &mesh, const VectorS<3> &center_before, bcg_scalar_t area_before);

void explicit_smoothing(halfedge_mesh &mesh, const mesh_laplacian &laplacian, int smoothing_steps = 1,bcg_scalar_t timestep = 0.5,
                        size_t parallel_grain_size = 1024);

void explicit_smoothing(halfedge_mesh &mesh, const mesh_laplacian &laplacian, property<bcg_scalar_t, 1> &property,
                        int smoothing_steps = 1, bcg_scalar_t timestep = 0.5, size_t parallel_grain_size = 1024);

void explicit_smoothing(halfedge_mesh &mesh, const mesh_laplacian &laplacian, property<VectorS<3>, 3> &property,
                        int smoothing_steps = 1, bcg_scalar_t timestep = 0.5, size_t parallel_grain_size = 1024);

void implicit_smoothing(halfedge_mesh &mesh, const mesh_laplacian &laplacian, bcg_scalar_t timestep);

void taubin_smoothing(halfedge_mesh &mesh, const mesh_laplacian &laplacian, bcg_scalar_t lambda, bcg_scalar_t mu, int smoothing_steps);

template<typename T, int N>
void
implicit_smoothing(halfedge_mesh &mesh, const mesh_laplacian &, property<T, N> &p, bcg_scalar_t timestep) {
    auto v_feature = mesh.vertices.get_or_add<bool, 1>("v_feature");
    auto e_weight = mesh.edges.get<bcg_scalar_t, 1>("e_laplacian_weight");
    auto v_weight = mesh.vertices.get<bcg_scalar_t, 1>("v_laplacian_weight");

    auto idx = mesh.vertices.add<int, 1>("v:idx", -1);

    // collect free (non-boundary) vertices in array free_vertices[]
    // assign indices such that idx[ free_vertices[i] ] == i
    unsigned j = 0;
    std::vector<vertex_handle> free_vertices;
    free_vertices.reserve(mesh.vertices.size());
    for (const auto v : mesh.vertices) {
        if ((v_feature && v_feature[v]) || mesh.is_boundary(v)) {

        } else {
            idx[v] = j++;
            free_vertices.push_back(v);
        }
    }
    const unsigned int n = free_vertices.size();

    // A*X = B
    SparseMatrix<bcg_scalar_t> A(n, n);
    MatrixS<-1, N> B(n, p.dims());
    auto P = Map(p);

    // nonzero elements of A as triplets: (row, column, value)
    std::vector<Eigen::Triplet<bcg_scalar_t>> triplets;

    // setup matrix A and rhs B

    bcg_scalar_t ww;
    vertex_handle v, vv;
    edge_handle e;
    for (unsigned int i = 0; i < n; ++i) {
        v = free_vertices[i];

        // rhs row
        B.row(i) = (P.row(v) / 4 * v_weight[v]);

        // lhs row
        ww = 0.0;
        for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
            vv = mesh.get_to_vertex(h);
            e = mesh.get_edge(h);
            ww += e_weight[e];

            // fixed boundary vertex -> right hand side
            if (mesh.is_boundary(vv)) {
                B.row(i) -= -timestep * e_weight[e] * P.row(vv);
            }
                // free interior vertex -> matrix
            else {
                triplets.emplace_back(i, idx[vv], -timestep * e_weight[e]);
            }
        }

        // center vertex -> matrix
        triplets.emplace_back(i, i, 1.0 / 4 * v_weight[v] + timestep * ww);
    }

    // build sparse matrix from triplets
    A.setFromTriplets(triplets.begin(), triplets.end());

    // solve A*X = B
    Eigen::SimplicialLDLT<SparseMatrix<bcg_scalar_t >> solver(A);
    MatrixS<-1, N> X = solver.solve(B);
    if (solver.info() != Eigen::Success) {
        std::cerr << "SurfaceSmoothing: Could not solve linear system\n";
    } else {
        // copy solution
        for (unsigned int i = 0; i < n; ++i) {
            Map(p).row(free_vertices[i]) = X.row(i);
        }
    }
    mesh.vertices.remove(idx);
    p.set_dirty();
}

}

#endif //BCG_GRAPHICS_BCG_MESH_SMOOTHING_H
