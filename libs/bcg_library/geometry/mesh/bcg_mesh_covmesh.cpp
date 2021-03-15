//
// Created by alex on 20.01.21.
//

#include "bcg_mesh_covmesh.h"
#include "tbb/tbb.h"

namespace bcg {

void mesh_convert_vertex_based_covmesh(halfedge_mesh &mesh, bcg_scalar_t kernel_sigma_0, size_t parallel_grain_size) {
    auto v_covs = mesh.vertices.get_or_add<MatrixS<3, 3>, 1>("v_covariance_matrices", MatrixS<3, 3>::Identity());
    MatrixS<3, 3> ID = MatrixS<3, 3>::Identity();
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    v_covs[v].setZero();
                    size_t count = 0;
                    for (const auto vh : mesh.halfedge_graph::get_halfedges(v)) {
                        VectorS<3> diff = mesh.positions[mesh.get_to_vertex(vh)] - mesh.positions[v];
                        v_covs[v] += diff * diff.transpose();
                        ++count;
                    }
                    v_covs[v] /= bcg_scalar_t(count);
                    v_covs[v] += kernel_sigma_0 * ID;
                }
            }
    );
}

void mesh_convert_face_based_covmesh(halfedge_mesh &mesh, bcg_scalar_t kernel_sigma_0, size_t parallel_grain_size) {
    auto v_covs = mesh.vertices.get_or_add<MatrixS<3, 3>, 1>("v_covariance_matrices", MatrixS<3, 3>::Identity());
    auto f_covs = mesh.faces.get_or_add<MatrixS<3, 3>, 1>("f_covariance_matrices", MatrixS<3, 3>::Identity());
    MatrixS<3, 3> ID = MatrixS<3, 3>::Identity();
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_covs[f].setZero();
                    size_t count = 0;
                    VectorS<3> center = VectorS<3>::Zero();
                    for (const auto vf : mesh.get_vertices(f)) {
                        center += mesh.positions[vf];
                        ++count;
                    }
                    center /= bcg_scalar_t(count);

                    for (const auto vf : mesh.get_vertices(f)) {
                        VectorS<3> diff = mesh.positions[vf] - center;
                        f_covs[f] += diff * diff.transpose();
                    }
                    f_covs[f] /= bcg_scalar_t(count);
                    f_covs[f] += kernel_sigma_0 * ID;
                }
            }
    );

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    v_covs[v].setZero();
                    for (const auto fv : mesh.get_faces(v)) {
                        v_covs[v] += f_covs[fv].inverse();
                    }
                    v_covs[v] = v_covs[v].inverse();
                }
            }
    );
}

void mesh_convert_gaussian_mixture_covmesh(halfedge_mesh &mesh, bcg_scalar_t kernel_sigma_0, size_t parallel_grain_size) {

}

void mesh_convert_hierarchical_gaussian_mixture_covmesh(halfedge_mesh &mesh, bcg_scalar_t kernel_sigma_0, size_t parallel_grain_size) {

}

}