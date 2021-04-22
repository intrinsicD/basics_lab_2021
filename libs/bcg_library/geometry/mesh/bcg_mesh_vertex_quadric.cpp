//
// Created by alex on 18.04.21.
//

#include "bcg_mesh_vertex_quadric.h"
#include "bcg_mesh_vertex_normals.h"
#include "bcg_mesh_vertex_area_voronoi.h"
#include "bcg_property_map_eigen.h"
#include "tbb/tbb.h"

namespace bcg {

std::vector<std::string> mesh_vertex_quadric_type_names() {
    std::vector<std::string> names(static_cast<int>(MeshVertexQuadricType::__last__));
    names[static_cast<int>(MeshVertexQuadricType::point)] = "point";
    names[static_cast<int>(MeshVertexQuadricType::plane)] = "plane";
    names[static_cast<int>(MeshVertexQuadricType::global_isotropic_probabilistic_plane)] = "global_isotropic_probabilistic_plane";
    names[static_cast<int>(MeshVertexQuadricType::global_anisotropic_probabilistic_plane)] = "global_anisotropic_probabilistic_plane";
    names[static_cast<int>(MeshVertexQuadricType::local_isotropic_probabilistic_plane)] = "local_isotropic_probabilistic_plane";
    names[static_cast<int>(MeshVertexQuadricType::local_anisotropic_probabilistic_plane)] = "local_anisotropic_probabilistic_plane";
    names.push_back("");
    return names;
}

void mesh_vertex_point_quadric(halfedge_mesh &mesh, size_t parallel_grain_size) {
    auto quadrics = mesh.vertices.get_or_add<quadric, 1>("v_quadric");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    quadrics[v] = quadric::point_quadric(mesh.positions[v]);
                }
            }
    );
}

void mesh_vertex_plane_quadric(halfedge_mesh &mesh, property<VectorS<3>, 3> normals, size_t parallel_grain_size) {
    auto quadrics = mesh.vertices.get_or_add<quadric, 1>("v_quadric");
    if (!normals) {
        normals = mesh.vertices.get<VectorS<3>, 3>("v_normal");
    }

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    quadrics[v] = quadric::plane_quadric(mesh.positions[v], normals[v]);
                }
            }
    );
}

//global isotropic sigmas
void mesh_vertex_probabilistic_plane_quadric_isotropic(halfedge_mesh &mesh, property<VectorS<3>, 3> normals,
                                                       bcg_scalar_t sigma_p,
                                                       bcg_scalar_t sigma_n, size_t parallel_grain_size) {
    auto quadrics = mesh.vertices.get_or_add<quadric, 1>("v_quadric");
    if (!normals) {
        normals = mesh.vertices.get<VectorS<3>, 3>("v_normal");
    }

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    quadrics[v] = quadric::probabilistic_plane_quadric(mesh.positions[v], normals[v], sigma_p, sigma_n);
                }
            }
    );
}

//global anisotropic sigmas
void
mesh_vertex_probabilistic_plane_quadric_anisotropic(halfedge_mesh &mesh, property<VectorS<3>, 3> normals,
                                                    const MatrixS<3, 3> &sigma_p, const MatrixS<3, 3> &sigma_n,
                                                    size_t parallel_grain_size) {
    auto quadrics = mesh.vertices.get_or_add<quadric, 1>("v_quadric");
    if (!normals) {
        normals = mesh.vertices.get<VectorS<3>, 3>("v_normal");
    }

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    quadrics[v] = quadric::probabilistic_plane_quadric(mesh.positions[v], normals[v], sigma_p, sigma_n);
                }
            }
    );
}

//local isotropic sigmas
void mesh_vertex_probabilistic_plane_quadric_isotropic(halfedge_mesh &mesh, property<VectorS<3>, 3> normals,
                                                       property<bcg_scalar_t, 1> sigma_p,
                                                       property<bcg_scalar_t, 1> sigma_n, size_t parallel_grain_size) {
    auto quadrics = mesh.vertices.get_or_add<quadric, 1>("v_quadric");
    if (!normals) {
        normals = mesh.vertices.get<VectorS<3>, 3>("v_normal");
    }

    if (!sigma_p) {
        sigma_p = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_position_isotropic_variance");
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);
                        MatrixS<3, 3> P = MatrixS<3, 3>::Zero();
                        bcg_scalar_t sum_weights = 0;
                        for (const auto &vv : mesh.halfedge_graph::get_vertices(v)) {
                            VectorS<3> diff = mesh.positions[vv] - mesh.positions[v];
                            P += diff * diff.transpose();
                            sum_weights += 1.0;
                        }
                        P /= sum_weights;
                        Eigen::JacobiSVD<MatrixS<3, 3>> svd(P, Eigen::ComputeFullV);
                        sigma_p[v] = svd.singularValues().mean();
                    }
                }
        );
        sigma_p.set_dirty();
        std::cout << "estimated sigma_p from neighborhood positions!\n";
    }
    if (!sigma_n) {
        sigma_n = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_normal_isotropic_variance");
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);
                        MatrixS<3, 3> N = MatrixS<3, 3>::Zero();
                        bcg_scalar_t sum_weights = 1;
                        for (const auto &vv : mesh.halfedge_graph::get_vertices(v)) {
                            VectorS<3> diff = normals[vv] - normals[v];
                            N += diff * diff.transpose();
                            sum_weights += 1.0;
                        }
                        N /= sum_weights;
                        Eigen::JacobiSVD<MatrixS<3, 3>> svd(N, Eigen::ComputeFullV);
                        sigma_n[v] = svd.singularValues().mean();
                    }
                }
        );
        sigma_n.set_dirty();
        std::cout << "estimated sigma_n from neighborhood normals!\n";
    }
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    quadrics[v] = quadric::probabilistic_plane_quadric(mesh.positions[v], normals[v], sigma_p[v],
                                                                       sigma_n[v]);
                }
            }
    );
}

//local anisotropic sigmas
void mesh_vertex_probabilistic_plane_quadric_anisotropic(halfedge_mesh &mesh, property<VectorS<3>, 3> normals,
                                                         property<MatrixS<3, 3>, 1> sigma_p,
                                                         property<MatrixS<3, 3>, 1> sigma_n,
                                                         size_t parallel_grain_size) {
    auto quadrics = mesh.vertices.get_or_add<quadric, 1>("v_quadric");
    if (!normals) {
        normals = mesh.vertices.get<VectorS<3>, 3>("v_normal");
    }

    if (!sigma_p) {
        sigma_p = mesh.vertices.get_or_add<MatrixS<3, 3>, 1>("v_position_isotropic_variance");
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);
                        MatrixS<3, 3> P = MatrixS<3, 3>::Zero();
                        bcg_scalar_t sum_weights = 0;
                        for (const auto &vv : mesh.halfedge_graph::get_vertices(v)) {
                            VectorS<3> diff = mesh.positions[vv] - mesh.positions[v];
                            P += diff * diff.transpose();
                            sum_weights += 1.0;
                        }
                        sigma_p[v] = P / sum_weights;
                    }
                }
        );
        sigma_p.set_dirty();
        std::cout << "estimated sigma_p from neighborhood positions!\n";
    }
    if (!sigma_n) {
        sigma_n = mesh.vertices.get_or_add<MatrixS<3, 3>, 1>("v_normal_isotropic_variance");
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);
                        MatrixS<3, 3> N = MatrixS<3, 3>::Zero();
                        bcg_scalar_t sum_weights = 0;
                        for (const auto &vv : mesh.halfedge_graph::get_vertices(v)) {
                            VectorS<3> diff = normals[vv] - normals[v];
                            N += diff * diff.transpose();
                            sum_weights += 1.0;
                        }
                        N /= sum_weights;
                        sigma_n[v] = N / sum_weights;
                    }
                }
        );
        sigma_n.set_dirty();
        std::cout << "estimated sigma_n from neighborhood normals!\n";
    }
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    quadrics[v] = quadric::probabilistic_plane_quadric(mesh.positions[v], normals[v], sigma_p[v],
                                                                       sigma_n[v]);
                }
            }
    );
}

//----------------------------------------------------------------------------------------------------------------------

void mesh_vertex_quadric_error(halfedge_mesh &mesh, property<quadric, 1> quadrics, size_t parallel_grain_size) {
    if (!quadrics) {
        std::cout << "Please compute vertex quadrics first!" << "\n";
        return;
    }
    auto quadric_error = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_quadric_error");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    quadric_error[v] = quadrics[v](mesh.positions[v]);
                }
            }
    );
    quadric_error.set_dirty();
}

void mesh_vertex_quadric_minimizer(halfedge_mesh &mesh, property<quadric, 1> quadrics,
                                   size_t parallel_grain_size) {
    if (!quadrics) {
        std::cout << "Please compute vertex quadrics first!" << "\n";
        return;
    }
    auto quadric_minimizer = mesh.vertices.get_or_add<VectorS<3>, 3>("v_quadric_minimizer");
    auto old_positions = mesh.vertices.get<VectorS<3>, 3>("v_position_old");
    if (!old_positions) {
        old_positions = mesh.vertices.get_or_add<VectorS<3>, 3>("v_position_old");
        Map(old_positions) = MapConst(mesh.positions);
    }
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    quadric_minimizer[v] = quadrics[v].minimizer();
                }
            }
    );
    if (old_positions) {
        Map(mesh.positions) = MapConst(quadric_minimizer);
        vertex_normals(mesh, vertex_normal_area_angle, parallel_grain_size);
        mesh.positions.set_dirty();
    }
    quadric_minimizer.set_dirty();
}


void mesh_vertex_quadric_neighbors_sum(halfedge_mesh &mesh, property<quadric, 1> quadrics, Params params,
                                       size_t parallel_grain_size) {
    if (!quadrics) {
        std::cout << "Please compute vertex quadrics first!" << "\n";
        return;
    }
    auto quadric_last = mesh.vertices.get_or_add<quadric, 1>("v_quadric_last");
    Map(quadric_last) = MapConst(quadrics);
    auto quadric_sum = mesh.vertices.get_or_add<quadric, 1>("v_quadric_sum");

    property<VectorS<3>, 3> normals;
    if (params.use_normal_weighting) {
        normals = mesh.vertices.get<VectorS<3>, 3>("v_normal");
    }
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    bcg_scalar_t weight = 1.0;
                    if (params.use_area_weighting) {
                        weight *= vertex_voronoi_area(mesh, v);
                    }
                    quadric_sum[v] = quadric_last[v] * weight;
                    for (const auto &vv : mesh.halfedge_graph::get_vertices(v)) {
                        weight = 1.0;
                        if (params.use_normal_weighting) {
                            weight *= std::exp(-(normals[vv] - normals[v]).norm());
                        }
                        if (params.use_area_weighting) {
                            weight *= vertex_voronoi_area(mesh, vv);
                        }
                        if (params.use_distance_weighting) {
                            weight *= std::exp(-(mesh.positions[vv] - mesh.positions[v]).norm());
                        }
                        quadric_sum[v] += quadric_last[vv] * weight;
                    }
                }
            }
    );
    quadric_sum.set_dirty();
}

void mesh_vertex_quadric_neighbors_avg(halfedge_mesh &mesh, property<quadric, 1> quadrics, Params params,
                                       size_t parallel_grain_size) {
    if (!quadrics) {
        std::cout << "Please compute vertex quadrics first!" << "\n";
        return;
    }
    auto quadric_last = mesh.vertices.get_or_add<quadric, 1>("v_quadric_last");
    Map(quadric_last) = MapConst(quadrics);
    auto quadric_avg = mesh.vertices.get_or_add<quadric, 1>("v_quadric_avg");
    property<VectorS<3>, 3> normals;
    if (params.use_normal_weighting) {
        normals = mesh.vertices.get<VectorS<3>, 3>("v_normal");
    }
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    bcg_scalar_t weight = 1.0;
                    if (params.use_area_weighting) {
                        weight *= vertex_voronoi_area(mesh, v);
                    }
                    quadric_avg[v] = quadric_last[v] * weight;
                    bcg_scalar_t sum_weights = weight;
                    for (const auto &vv : mesh.halfedge_graph::get_vertices(v)) {
                        weight = 1.0;
                        if (params.use_normal_weighting) {
                            weight *= std::exp(-(normals[vv] - normals[v]).norm());
                        }
                        if (params.use_area_weighting) {
                            weight *= vertex_voronoi_area(mesh, vv);
                        }
                        if (params.use_distance_weighting) {
                            weight *= std::exp(-(mesh.positions[vv] - mesh.positions[v]).norm());
                        }

                        quadric_avg[v] += quadric_last[vv] * weight;
                        sum_weights += weight;
                    }
                    quadric_avg[v] /= sum_weights;
                }
            }
    );
    quadric_avg.set_dirty();
}

void
mesh_vertex_quadric_extract_normals(halfedge_mesh &mesh, property<quadric, 1> quadrics, size_t parallel_grain_size) {
    if (!quadrics) {
        std::cout << "Please compute vertex quadrics first!" << "\n";
        return;
    }
    auto quadric_normals = mesh.vertices.get_or_add<VectorS<3>, 3>("v_quadric_normal");
    auto quadric_normals_dir1 = mesh.vertices.get_or_add<VectorS<3>, 3>("v_quadric_normals_dir1");
    auto quadric_normals_dir2 = mesh.vertices.get_or_add<VectorS<3>, 3>("v_quadric_normals_dir2");
    auto quadric_normals_loadings = mesh.vertices.get_or_add<VectorS<3>, 3>("v_quadric_normals_loading");
    auto quadric_normals_loadings_0 = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_quadric_normals_loading_0");
    auto quadric_normals_loadings_1 = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_quadric_normals_loading_1");
    auto quadric_normals_loadings_2 = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_quadric_normals_loading_2");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    Eigen::JacobiSVD<MatrixS<3, 3>> svd(quadrics[v].A(), Eigen::ComputeFullV);
                    quadric_normals[v] = svd.matrixV().col(0);
                    quadric_normals_dir1[v] = svd.matrixV().col(1);
                    quadric_normals_dir2[v] = svd.matrixV().col(2);
                    quadric_normals_loadings[v] = svd.singularValues();
                    quadric_normals_loadings_0[v] = svd.singularValues()[0];
                    quadric_normals_loadings_1[v] = svd.singularValues()[1];
                    quadric_normals_loadings_2[v] = svd.singularValues()[2];
                }
            }
    );
    quadric_normals.set_dirty();
    quadric_normals_dir1.set_dirty();
    quadric_normals_dir2.set_dirty();
    quadric_normals_loadings.set_dirty();
    quadric_normals_loadings_0.set_dirty();
    quadric_normals_loadings_1.set_dirty();
    quadric_normals_loadings_2.set_dirty();
}

}