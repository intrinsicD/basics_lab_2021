//
// Created by alex on 01.12.20.
//

#include "bcg_gui_point_cloud_curvature_taubin.h"
#include "math/bcg_vertex_classify_curvature.h"
#include "bcg_property_map_eigen.h"
#include "tbb/tbb.h"
#include "Eigen/Eigenvalues"

namespace bcg {

void
point_cloud_curvature_taubin(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index, int num_closest,
                             size_t parallel_grain_size) {
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto min = vertices->get_or_add<bcg_scalar_t, 1>("v_pc_curv_min");
    auto max = vertices->get_or_add<bcg_scalar_t, 1>("v_pc_curv_max");
    auto mean = vertices->get_or_add<bcg_scalar_t, 1>("v_pc_curv_mean");
    auto gauss = vertices->get_or_add<bcg_scalar_t, 1>("v_pc_curv_gauss");
    auto min_direction = vertices->get_or_add<VectorS<3>, 3>("v_pc_curv_min_dir");
    auto max_direction = vertices->get_or_add<VectorS<3>, 3>("v_pc_curv_max_dir");
    if (!vertices->has("v_pca_loading")) {
        std::cout << "please compute vertex pca first!\n";
        return;
    }
    auto v_pca_loading = vertices->get<VectorS<3>, 3>("v_pca_loading");
    auto normals = vertices->get<VectorS<3>, 3>("v_normal");
    if (!normals || MapConst(normals).sum() == 0) {
        normals = vertices->get<VectorS<3>, 3>("v_pca_normal");
    }

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    bcg_scalar_t kmin = 0.0;
                    bcg_scalar_t kmax = 0.0;

                    auto result = index.query_knn(positions[v], num_closest);
                    MatrixS<3, 3> tensor(MatrixS<3, 3>::Zero());
                    bcg_scalar_t H = v_pca_loading[v][2] / v_pca_loading[v].sum();

                    for (int j = 0; j < num_closest; ++j) {
                        VectorS<3> ev = positions[v] - positions[result.indices[j]];
                        bcg_scalar_t l = ev.norm();
                        if (l == 0) continue;

                        bcg_scalar_t beta = (2 * normals[v].dot(ev) / (l * l) - H);
                        ev /= l;
                        l *= 0.5;
                        ev *= sqrt(l);

/*                        bcg_scalar_t sign = normals[v].dot(normals[result.indices[j]]) < 0 ? -1 : 1;
                        bcg_scalar_t beta = (2 * (normals[v] - sign * normals[result.indices[j]]).dot(ev) / (l * l));
                        ev /= l;
                        l *= 0.5;
                        ev *= sqrt(l);*/

                        tensor += beta * ev * ev.transpose();
                    }
                    bcg_scalar_t A = v_pca_loading[v][0] * v_pca_loading[v][1];
                    tensor /= A;

                    Eigen::SelfAdjointEigenSolver<MatrixS<3, 3>> solver(tensor);

                    if (solver.info() == Eigen::Success) {
                        // curvature values:
                        //   normal vector -> eval with smallest absolute value
                        //   evals are sorted in decreasing order
                        VectorS<-1> evals = solver.eigenvalues();

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
                    assert(kmin <= kmax);

                    min[v] = kmin;
                    max[v] = kmax;
                    gauss[v] = kmin * kmax;
                    mean[v] = (kmin + kmax) / 2;
                }
            }
    );
    min.set_dirty();
    max.set_dirty();
    min_direction.set_dirty();
    max_direction.set_dirty();
    mean.set_dirty();
    gauss.set_dirty();

    vertex_classify_curvature(vertices, min, max, parallel_grain_size);
}

void point_cloud_curvature_taubin(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index,
                                  bcg_scalar_t radius, size_t parallel_grain_size) {
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto min = vertices->get_or_add<bcg_scalar_t, 1>("v_pc_curv_min");
    auto max = vertices->get_or_add<bcg_scalar_t, 1>("v_pc_curv_max");
    auto mean = vertices->get_or_add<bcg_scalar_t, 1>("v_pc_curv_mean");
    auto gauss = vertices->get_or_add<bcg_scalar_t, 1>("v_pc_curv_gauss");
    auto min_direction = vertices->get_or_add<VectorS<3>, 3>("v_pc_curv_min_dir");
    auto max_direction = vertices->get_or_add<VectorS<3>, 3>("v_pc_curv_max_dir");
    if (!vertices->has("v_pca_loading")) {
        std::cout << "please compute vertex pca first!\n";
        return;
    }
    auto v_pca_loading = vertices->get<VectorS<3>, 3>("v_pca_loading");
    auto normals = vertices->get<VectorS<3>, 3>("v_normal");
    if (!normals || MapConst(normals).sum() == 0) {
        normals = vertices->get<VectorS<3>, 3>("v_pca_normal");
    }

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    bcg_scalar_t kmin = 0.0;
                    bcg_scalar_t kmax = 0.0;

                    auto result = index.query_radius(positions[v], radius);
                    MatrixS<3, 3> tensor(MatrixS<3, 3>::Zero());
                    bcg_scalar_t H = v_pca_loading[v][2] / v_pca_loading[v].sum();
                    size_t size = result.indices.size();
                    for (int j = 0; j < size; ++j) {
                        VectorS<3> ev = positions[result.indices[j]] - positions[v];
                        bcg_scalar_t l = ev.norm();
                        if (l == 0) continue;

                        bcg_scalar_t beta = (2 * normals[v].dot(ev) / (l * l) - H);
                        ev /= l;
                        l *= 0.5;
                        ev *= sqrt(l);

/*                        bcg_scalar_t sign = normals[v].dot(normals[result.indices[j]]) < 0 ? -1 : 1;
                        bcg_scalar_t beta = (2 * (normals[v] - sign * normals[result.indices[j]]).dot(ev) / (l * l));
                        ev /= l;
                        l *= 0.5;
                        ev *= sqrt(l);*/

                        tensor += beta * ev * ev.transpose();
                    }
                    bcg_scalar_t A = v_pca_loading[v][0] * v_pca_loading[v][1];
                    tensor /= A;

                    Eigen::SelfAdjointEigenSolver<MatrixS<3, 3>> solver(tensor);

                    if (solver.info() == Eigen::Success) {
                        // curvature values:
                        //   normal vector -> eval with smallest absolute value
                        //   evals are sorted in decreasing order
                        VectorS<-1> evals = solver.eigenvalues();

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
                    min[v] = kmin;
                    max[v] = kmax;
                    gauss[v] = kmin * kmax;
                    mean[v] = (kmin + kmax) / 2;
                }
            }
    );
    min.set_dirty();
    max.set_dirty();
    min_direction.set_dirty();
    max_direction.set_dirty();
    mean.set_dirty();
    gauss.set_dirty();

    vertex_classify_curvature(vertices, min, max, parallel_grain_size);
}

}