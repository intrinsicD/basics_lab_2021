//
// Created by alex on 23.11.20.
//

#include "bcg_mesh_laplacian.h"
#include "bcg_mesh_edge_cotan.h"
#include "bcg_mesh_edge_fujiwara.h"
#include "bcg_mesh_vertex_area_voronoi.h"
#include "bcg_mesh_vertex_cotan.h"
#include "bcg_mesh_vertex_area_barycentric.h"
#include "math/sparse_matrix/bcg_sparse_check_symmetric.h"
#include "tbb/tbb.h"

namespace bcg {

std::vector<std::string> mesh_laplacian_stiffness_type() {
    std::vector<std::string> names(static_cast<unsigned int>(MeshLaplacianStiffness::__last__));
    names[static_cast<unsigned int>(MeshLaplacianStiffness::uniform)] = "uniform";
    names[static_cast<unsigned int>(MeshLaplacianStiffness::cotan)] = "cotan";
    names[static_cast<unsigned int>(MeshLaplacianStiffness::fujiwara)] = "fujiwara";
    return names;
}

std::vector<std::string> mesh_laplacian_mass_type() {
    std::vector<std::string> names(static_cast<unsigned int>(MeshLaplacianMass::__last__));
    names[static_cast<unsigned int>(MeshLaplacianMass::uniform)] = "uniform";
    names[static_cast<unsigned int>(MeshLaplacianMass::cotan)] = "cotan";
    names[static_cast<unsigned int>(MeshLaplacianMass::fujiwara)] = "fujiwara";
    names[static_cast<unsigned int>(MeshLaplacianMass::voronoi)] = "voronoi";
    names[static_cast<unsigned int>(MeshLaplacianMass::barycentric)] = "barycentric";
    return names;
}

std::vector<Eigen::Triplet<bcg_scalar_t>>
compute_edge_weights(halfedge_mesh &mesh, MeshLaplacianStiffness s_type, property<bcg_scalar_t, 1> e_scaling,
                     size_t parallel_grain_size) {
    property<bcg_scalar_t, 1> eweight = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_laplacian_weight");
    switch (s_type) {
        case MeshLaplacianStiffness::uniform : {
            eweight.vector() = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_laplacian_uniform", 1.0).vector();
            break;
        }
        case MeshLaplacianStiffness::cotan : {
            edge_cotans(mesh, parallel_grain_size);
            eweight.vector() = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_cotan").vector();
            break;
        }
        case MeshLaplacianStiffness::fujiwara : {
            edge_fujiwaras(mesh, parallel_grain_size);
            eweight.vector() = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_fujiwara").vector();
            break;
        }
        case MeshLaplacianStiffness::__last__ : {
            return std::vector<Eigen::Triplet<bcg_scalar_t>>();
        }
    }

    tbb::concurrent_vector<Eigen::Triplet<bcg_scalar_t>> e_coeffs;

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t k = range.begin(); k != range.end(); ++k) {
                    auto e = edge_handle(k);
                    if (e_scaling) eweight[e] *= e_scaling[e];

                    const auto i = mesh.get_vertex(e, 0).idx;
                    const auto j = mesh.get_vertex(e, 1).idx;

                    e_coeffs.emplace_back(i, j, eweight[e]);
                    e_coeffs.emplace_back(j, i, eweight[e]);
                    e_coeffs.emplace_back(i, i, -eweight[e]);
                    e_coeffs.emplace_back(j, j, -eweight[e]);
                }
            }
    );
    std::vector<Eigen::Triplet<bcg_scalar_t>> result(e_coeffs.size());
    std::copy(e_coeffs.begin(), e_coeffs.end(), result.begin());
    return result;
}
//TODO fix me, edge scaling should probably not be in vertex things!

void vertex_from_edges(halfedge_mesh &mesh, property<bcg_scalar_t, 1> e_weight, property<bcg_scalar_t, 1> e_scaling,
                       size_t parallel_grain_size) {
    property<bcg_scalar_t, 1> vweight = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_laplacian_weight");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    bcg_scalar_t weight = 0;
                    for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
                        auto e = mesh.get_edge(h);
                        weight += e_weight[e] * (e_scaling ? e_scaling[e] : 1.0);
                    }
                    vweight[v] = weight;
                }
            }
    );
    vweight.set_dirty();
}

void vertex_uniform(halfedge_mesh &mesh, size_t parallel_grain_size) {
    property<bcg_scalar_t, 1> vweight = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_laplacian_uniform");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    vweight[v] = mesh.halfedge_graph::get_valence(v);
                }
            }
    );
    vweight.set_dirty();
}

void vertex_fujuwara(halfedge_mesh &mesh, size_t parallel_grain_size) {
    property<bcg_scalar_t, 1> vweight = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_laplacian_fujiwara");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    bcg_scalar_t weight = 0;
                    for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
                        auto e = mesh.get_edge(h);
                        weight += std::max<bcg_scalar_t>(mesh.get_length(e), scalar_eps);
                    }
                    if (weight != 0) {
                        vweight[v] = 1.0 / weight;
                    }
                }
            }
    );
    vweight.set_dirty();
}

std::vector<Eigen::Triplet<bcg_scalar_t>>
compute_vertex_weights(halfedge_mesh &mesh, MeshLaplacianMass m_type, size_t parallel_grain_size) {
    property<bcg_scalar_t, 1> eweight = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_laplacian_weight");
    property<bcg_scalar_t, 1> vweight;
    switch (m_type) {
        case MeshLaplacianMass::uniform : {
            vertex_uniform(mesh, parallel_grain_size);
            vweight = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_laplacian_weight");
            vweight.vector() = mesh.vertices.get<bcg_scalar_t, 1>("v_laplacian_uniform").vector();
            break;
        }
        case MeshLaplacianMass::cotan : {
            vertex_cotans(mesh, parallel_grain_size);
            vweight = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_laplacian_weight");
            vweight.vector() = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_laplacian_cotan").vector();
            break;
        }
        case MeshLaplacianMass::fujiwara : {
            vertex_fujuwara(mesh, parallel_grain_size);
            vweight = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_laplacian_weight");
            vweight.vector() = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_laplacian_fujiwara").vector();
            break;
        }
        case MeshLaplacianMass::voronoi : {
            vertex_voronoi_areas(mesh, parallel_grain_size);
            vweight = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_laplacian_weight");
            vweight.vector() = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_voronoi_area").vector();
            break;
        }
        case MeshLaplacianMass::barycentric : {
            vertex_barycentric_areas(mesh, parallel_grain_size);
            vweight = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_laplacian_weight");
            vweight.vector() = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_barycentric_area").vector();
            break;
        }
        case MeshLaplacianMass::__last__ : {
            return std::vector<Eigen::Triplet<bcg_scalar_t>>();
        }
    }

    tbb::concurrent_vector<Eigen::Triplet<bcg_scalar_t>> v_coeffs;

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t k = range.begin(); k != range.end(); ++k) {
                    auto v = vertex_handle(k);
                    v_coeffs.emplace_back(v.idx, v.idx, vweight[v]);
                }
            }
    );

    std::vector<Eigen::Triplet<bcg_scalar_t>> result(v_coeffs.size());
    std::copy(v_coeffs.begin(), v_coeffs.end(), result.begin());
    return result;
}

mesh_laplacian build_laplacian(halfedge_mesh &mesh, MeshLaplacianStiffness s_type, MeshLaplacianMass m_type,
                               size_t parallel_grain_size,
                               std::string edge_scaling_property_name) {
    property<bcg_scalar_t, 1> e_scaling;
    if (mesh.edges.has(edge_scaling_property_name)) {
        e_scaling = mesh.edges.get<bcg_scalar_t, 1>(edge_scaling_property_name);
    }
    auto e_coeffs = compute_edge_weights(mesh, s_type, e_scaling, parallel_grain_size);
    auto v_coeffs = compute_vertex_weights(mesh, m_type, parallel_grain_size);

    auto N = mesh.vertices.size();

    SparseMatrix<bcg_scalar_t> S, M;

    S.resize(N, N);
    M.resize(N, N);

    S.setFromTriplets(e_coeffs.begin(), e_coeffs.end());
    M.setFromTriplets(v_coeffs.begin(), v_coeffs.end());

    check_symmetric(S, scalar_eps);

    std::cout << "(Const to zero) S * 1 = " << std::to_string((S * VectorS<-1>::Ones(N)).sum()) << "\n";

    mesh_laplacian lap;
    lap.s_type = s_type;
    lap.m_type = m_type;
    lap.S = S;
    lap.M = M;

    return lap;
}

}