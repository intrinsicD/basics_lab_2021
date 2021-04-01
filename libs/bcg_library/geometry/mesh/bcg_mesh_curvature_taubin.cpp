//
// Created by alex on 23.11.20.
//

#include "bcg_mesh_curvature_taubin.h"
#include "bcg_mesh_face_normals.h"
#include "bcg_mesh_vertex_area_voronoi.h"
#include "bcg_mesh_edge_cotan.h"
#include "math/bcg_vertex_classify_curvature.h"
#include "Eigen/Eigenvalues"
#include "tbb/tbb.h"

namespace bcg {

void post_smoothing_curvature(halfedge_mesh &mesh, int post_smoothing_steps, size_t parallel_grain_size) {
    // properties
    auto v_feature = mesh.vertices.get_or_add<bool, 1>("v_feature");

    auto min_curvature = mesh.vertices.get<bcg_scalar_t, 1>("v_mesh_curv_min");
    auto max_curvature = mesh.vertices.get<bcg_scalar_t, 1>("v_mesh_curv_max");
    auto gauss_curvature = mesh.vertices.get<bcg_scalar_t, 1>("v_mesh_curv_gauss");
    auto mean_curvature = mesh.vertices.get<bcg_scalar_t, 1>("v_mesh_curv_mean");

    // precompute cotan weight per edge
    edge_cotans(mesh, parallel_grain_size);

    auto e_cotan = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_cotan");

    for (int i = 0; i < post_smoothing_steps; ++i) {
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);
                        if (v_feature && v_feature[v]) {

                        } else {
                            bcg_scalar_t kmin = 0.0;
                            bcg_scalar_t kmax = 0.0;
                            bcg_scalar_t sum_weights = 0.0;

                            for (const auto vh : mesh.halfedge_graph::get_halfedges(v)) {
                                auto tv = mesh.get_to_vertex(vh);

                                // don't consider feature vertices (high curvature)
                                if (v_feature && v_feature[tv]) {
                                    continue;
                                }

                                bcg_scalar_t weight = fmax(0.0f, e_cotan[mesh.get_edge(vh)]);
                                sum_weights += weight;
                                kmin += weight * min_curvature[tv];
                                kmax += weight * max_curvature[tv];
                            }

                            if (sum_weights) {
                                min_curvature[v] = kmin / sum_weights;
                                max_curvature[v] = kmax / sum_weights;
                            }
                        }

                        gauss_curvature[v] = min_curvature[v] * max_curvature[v];
                        mean_curvature[v] = (min_curvature[v] + max_curvature[v]) / 2.0;
                    }
                }
        );
    }

    min_curvature.set_dirty();
    max_curvature.set_dirty();
    gauss_curvature.set_dirty();
    mean_curvature.set_dirty();
}

void mesh_curvature_taubin(halfedge_mesh &mesh, int post_smoothing_steps, bool two_ring_neighborhood,
                           size_t parallel_grain_size) {
    auto evec = mesh.edges.get_or_add<VectorS<3>, 3>("e_mesh_curv_evec");
    auto angle = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_mesh_curv_angle");

    auto min_curvature = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_mesh_curv_min");
    auto max_curvature = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_mesh_curv_max");
    auto gauss_curvature = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_mesh_curv_gauss");
    auto mean_curvature = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_mesh_curv_mean");
    auto min_direction = mesh.vertices.get_or_add<VectorS<3>, 3>("v_mesh_curv_min_dir");
    auto max_direction = mesh.vertices.get_or_add<VectorS<3>, 3>("v_mesh_curv_max_dir");

    // precompute Voronoi area per vertex
    vertex_voronoi_areas(mesh, parallel_grain_size);

    auto v_voronoi_area = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_voronoi_area");
    // precompute face normals
    face_normals(mesh, parallel_grain_size);

    auto f_normal = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal");


    // precompute dihedralAngle*edge_length*edge per edge
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    auto h0 = mesh.halfedge_graph::get_halfedge(e, 0);
                    auto h1 = mesh.halfedge_graph::get_halfedge(e, 1);
                    auto f0 = mesh.get_face(h0);
                    auto f1 = mesh.get_face(h1);
                    if (f0.is_valid() && f1.is_valid()) {
                        VectorS<3> ev = -mesh.halfedge_graph::get_vector(h0);
                        bcg_scalar_t l = ev.norm();
                        ev /= l;
                        l *= 0.5; // only consider half of the edge (matchig Voronoi area)
                        angle[e] = atan2(f_normal[f0].cross(f_normal[f1]).dot(ev), f_normal[f0].dot(f_normal[f1]));
                        evec[e] = sqrt(l) * ev;
                    }
                }
            }
    );

    // compute curvature tensor for each vertex
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    bcg_scalar_t kmin = 0.0;
                    bcg_scalar_t kmax = 0.0;
                    std::vector<vertex_handle> neighborhood;
                    neighborhood.reserve(15);

                    if (!mesh.is_isolated(v)) {
                        // one-ring or two-ring neighborhood?
                        neighborhood.clear();
                        neighborhood.push_back(v);
                        if (two_ring_neighborhood) {
                            for (const auto vv : mesh.halfedge_graph::get_vertices(v)) {
                                neighborhood.push_back(vv);
                            }
                        }

                        bcg_scalar_t A = 0.0;
                        MatrixS<3, 3> tensor(MatrixS<3, 3>::Zero());

                        // compute tensor over vertex neighborhood stored in vertices
                        for (const auto &nit : neighborhood) {
                            // accumulate tensor from dihedral angles around vertices
                            for (const auto hv : mesh.halfedge_graph::get_halfedges(nit)) {
                                auto ee = mesh.get_edge(hv);
                                VectorS<3> ev = evec[ee];
                                bcg_scalar_t beta = angle[ee];
                                for (int k = 0; k < 3; ++k) {
                                    for (int j = 0; j < 3; ++j) {
                                        tensor(k, j) += beta * ev[k] * ev[j];
                                    }
                                }
                            }

                            // accumulate area
                            A += v_voronoi_area[nit];
                        }

                        // normalize tensor by accumulated
                        tensor /= A;

                        // Eigen-decomposition
                        /*         bool ok = symmetric_eigendecomposition(tensor, eval1, eval2, eval3,
                                                                        evec1, evec2, evec3);*/

                        Eigen::SelfAdjointEigenSolver<MatrixS<3, 3>> solver(tensor);

                        if (solver.info() == Eigen::Success) {
                            // curvature values:
                            //   normal vector -> eval with smallest absolute value
                            //   evals are sorted in decreasing order
                            VectorS<3> evals = solver.eigenvalues();
                            bcg_scalar_t eval1 = evals[0];
                            bcg_scalar_t eval2 = evals[1];
                            bcg_scalar_t eval3 = evals[2];

                            VectorS<3> evec1 = solver.eigenvectors().col(0);
                            VectorS<3> evec2 = solver.eigenvectors().col(1);
                            VectorS<3> evec3 = solver.eigenvectors().col(2);

                            bcg_scalar_t a1 = std::abs(eval1);
                            bcg_scalar_t a2 = std::abs(eval2);
                            bcg_scalar_t a3 = std::abs(eval3);
                            if (a1 < a2) {
                                if (a1 < a3) {
                                    // e1 is normal
                                    kmin = eval2;
                                    kmax = eval3;
                                    min_direction[v] = evec2;
                                    max_direction[v] = evec3;
                                } else {
                                    // e3 is normal
                                    kmin = eval1;
                                    kmax = eval2;
                                    min_direction[v] = evec1;
                                    max_direction[v] = evec2;
                                }
                            } else {
                                if (a2 < a3) {
                                    // e2 is normal
                                    kmin = eval1;
                                    kmax = eval3;
                                    min_direction[v] = evec1;
                                    max_direction[v] = evec3;
                                } else {
                                    // e3 is normal
                                    kmin = eval1;
                                    kmax = eval2;
                                    min_direction[v] = evec1;
                                    max_direction[v] = evec2;
                                }
                            }
                        }
                    }

                    assert(kmin <= kmax);

                    min_curvature[v] = kmin;
                    max_curvature[v] = kmax;

                    gauss_curvature[v] = kmin * kmax;
                    mean_curvature[v] = (kmin + kmax) / 2.0;
                }
            }
    );

    // clean-up properties
    mesh.edges.remove(evec);
    mesh.edges.remove(angle);

    min_direction.set_dirty();
    max_direction.set_dirty();
    min_curvature.set_dirty();
    max_curvature.set_dirty();
    gauss_curvature.set_dirty();
    mean_curvature.set_dirty();

    // smooth curvature values
    post_smoothing_curvature(mesh, post_smoothing_steps, parallel_grain_size);

    vertex_classify_curvature(&mesh.vertices, min_curvature, max_curvature, parallel_grain_size);
}

}