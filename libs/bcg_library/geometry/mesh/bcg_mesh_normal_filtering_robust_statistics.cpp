//
// Created by alex on 17.02.21.
//

#include "bcg_mesh_normal_filtering_robust_statistics.h"
#include "bcg_mesh_face_centers.h"
#include "bcg_mesh_face_normals.h"
#include "bcg_mesh_face_areas.h"
#include "bcg_mesh_vertex_normals.h"
#include "bcg_property_map_eigen.h"
#include "bcg_mesh_curvature_taubin.h"
#include "geometry/quadric/bcg_quadric.h"
#include "math/vector/bcg_vector_median_filter_directional.h"
#include "tbb/tbb.h"

namespace bcg {

std::vector<std::string> normal_filtering_names() {
    std::vector<std::string> names(static_cast<int>(NormalFilteringType::__last__));
    names[static_cast<int>(NormalFilteringType::unilateral_belyaev_ohtake)] = "unilateral_belyaev_ohtake";
    names[static_cast<int>(NormalFilteringType::unilateral_yagou_mean)] = "unilateral_yagou_mean";
    names[static_cast<int>(NormalFilteringType::unilateral_yagou_median)] = "unilateral_yagou_median";
    names[static_cast<int>(NormalFilteringType::unilateral_yadav)] = "unilateral_yadav";
    names[static_cast<int>(NormalFilteringType::unilateral_shen)] = "unilateral_shen";
    names[static_cast<int>(NormalFilteringType::unilateral_tasdizen)] = "unilateral_tasdizen";
    names[static_cast<int>(NormalFilteringType::unilateral_centin)] = "unilateral_centin";
    names[static_cast<int>(NormalFilteringType::unilateral_probabilistic_quadric)] = "unilateral_probabilistic_quadric";
    names[static_cast<int>(NormalFilteringType::bilateral_zheng)] = "bilateral_zheng";
    names[static_cast<int>(NormalFilteringType::bilateral_zhang)] = "bilateral_zhang";
    names[static_cast<int>(NormalFilteringType::bilateral_yadav)] = "bilateral_yadav";
    return names;
}

void mesh_postprocessing(halfedge_mesh &mesh, bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, bool quadric_update,
                         size_t parallel_grain_size) {
    auto positions = mesh.vertices.get<VectorS<3>, 3>("v_position");
    auto normals = mesh.vertices.get<VectorS<3>, 3>("v_normal");
    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered");

    if (quadric_update) {
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);

                        quadric Q_total;
                        Q_total.probabilistic_plane_quadric(positions[v], normals[v], sigma_p, sigma_n);
                        for (const auto &fv : mesh.get_faces(v)) {
/*                        std::vector<VectorS<3>> V;
                        for (const auto vf : mesh.get_vertices(fv)) {
                            V.push_back(positions[vf]);
                        }
                        quadric Q;
                        Q.probabilistic_triangle_quadric(V[0], V[1], V[2], sigma_p);
                        Q_total += Q;*/
                            quadric Q;
                            Q.probabilistic_plane_quadric(face_center(mesh, fv), f_normals_filtered[fv], sigma_p,
                                                          sigma_n);
                            Q_total += Q;
                        }

                        if (mesh.is_boundary(v)) {
                            VectorS<3> delta = Q_total.minimizer() - positions[v];
                            positions[v] += Q_total.A() * delta * std::exp(-sigma_n);
                        } else {
                            positions[v] = Q_total.minimizer();
                        }
                    }
                }
        );
    } else {
        auto new_positions = mesh.vertices.get_or_add<VectorS<3>, 3>("v_position_new");
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);

                        VectorS<3> delta = VectorS<3>::Zero();
                        bcg_scalar_t sum_weights = 0;
                        for (const auto &fv : mesh.get_faces(v)) {
                            VectorS<3> PC = face_center(mesh, fv) - positions[v];
                            bcg_scalar_t weight = face_area(mesh, fv);
                            sum_weights += weight;
                            delta += weight * PC.dot(f_normals_filtered[fv]) * f_normals_filtered[fv];
                        }
                        delta /= sum_weights;
                        new_positions[v] = positions[v] + delta;
                    }
                }
        );
        Map(positions) = MapConst(new_positions);
    }

    face_normals(mesh, parallel_grain_size);
    vertex_normals(mesh, vertex_normal_area_angle, parallel_grain_size);
    positions.set_dirty();
    mesh.faces.remove(f_normals_filtered);
}

void mesh_normal_unilateral_filtering_belyaev_ohtake(halfedge_mesh &mesh,
                                                     bcg_scalar_t sigma_g,
                                                     bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                                     bool use_quadric_update,
                                                     size_t parallel_grain_size) {
    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered", VectorS<3>::Zero());

    bcg_scalar_t sigma_g_squared = sigma_g * sigma_g;
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    VectorS<3> n_i = face_normal(mesh, f);
                    VectorS<3> c_i = face_center(mesh, f);
                    f_normals_filtered[f] = face_area(mesh, f) * n_i;
                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            VectorS<3> n_j = face_normal(mesh, ff);
                            bcg_scalar_t x = acos(1.0 - (n_i - n_j).squaredNorm() / 2.0) /
                                             (c_i - face_center(mesh, ff)).norm();
                            bcg_scalar_t weight = std::exp(-x * x / sigma_g_squared) * face_area(mesh, ff);
                            f_normals_filtered[f] += weight * n_j;
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );

    mesh_postprocessing(mesh, sigma_p, sigma_n, use_quadric_update, parallel_grain_size);
}

void mesh_normal_unilateral_filtering_yagou_mean(halfedge_mesh &mesh,
                                                 bcg_scalar_t,
                                                 bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, bool use_quadric_update,
                                                 size_t parallel_grain_size) {
    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered", VectorS<3>::Zero());

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_normals_filtered[f] = face_area(mesh, f) * face_normal(mesh, f);
                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            f_normals_filtered[f] += face_area(mesh, ff) * face_normal(mesh, ff);
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );

    mesh_postprocessing(mesh, sigma_p, sigma_n, use_quadric_update, parallel_grain_size);
}

void mesh_normal_unilateral_filtering_yagou_median(halfedge_mesh &mesh,
                                                   bcg_scalar_t sigma_g,
                                                   bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, bool use_quadric_update,
                                                   size_t parallel_grain_size) {

    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered", VectorS<3>::Zero());

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    VectorS<3> n_i = face_normal(mesh, f);
                    f_normals_filtered[f] = face_area(mesh, f) * n_i;
                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            auto e = mesh.get_edge(fh);
                            VectorS<3> n_j = face_normal(mesh, ff);
                            bcg_scalar_t x = (n_i - n_j).norm();
                            bcg_scalar_t weight = 0;
                            if (x < sigma_g) {
                                if (x > scalar_eps) {
                                    weight = 1.0 / x;
                                } else {
                                    weight = 1.0;
                                }
                            }

                            f_normals_filtered[f] += weight * face_area(mesh, ff) * n_j;
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );

    mesh_postprocessing(mesh, sigma_p, sigma_n, use_quadric_update, parallel_grain_size);
}

void mesh_normal_unilateral_filtering_yadav(halfedge_mesh &mesh,
                                            bcg_scalar_t sigma_g,
                                            bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, bool use_quadric_update,
                                            size_t parallel_grain_size) {

    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered", VectorS<3>::Zero());

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    VectorS<3> n_i = face_normal(mesh, f);
                    f_normals_filtered[f] = face_area(mesh, f) * n_i;
                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            VectorS<3> n_j = face_normal(mesh, ff);
                            bcg_scalar_t x = acos(1.0 - (n_i - n_j).squaredNorm() / 2.0);
                            bcg_scalar_t weight = 0.1;
                            if (x < sigma_g) {
                                weight = 1.0;
                            }
                            f_normals_filtered[f] += weight * face_area(mesh, ff) * n_j;
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );

    mesh_postprocessing(mesh, sigma_p, sigma_n, use_quadric_update, parallel_grain_size);
}

void mesh_normal_unilateral_filtering_shen(halfedge_mesh &mesh,
                                           bcg_scalar_t sigma_g,
                                           bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, bool use_quadric_update,
                                           size_t parallel_grain_size) {
    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered", VectorS<3>::Zero());

    bcg_scalar_t sigma_g_squared = sigma_g * sigma_g;
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_normals_filtered[f].setZero();
                    std::vector<VectorS<3>> normals;
                    normals.push_back(face_normal(mesh, f));
                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            normals.push_back(face_normal(mesh, ff));
                        }
                    }
                    VectorS<3> median = vector_median_filter_directional(normals);
                    f_normals_filtered[f] = median * face_area(mesh, f);
                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            VectorS<3> n_j = face_normal(mesh, ff);
                            bcg_scalar_t x = (median - n_j).norm();
                            bcg_scalar_t weight = std::exp(-x * x / sigma_g_squared);
                            f_normals_filtered[f] += weight * face_area(mesh, ff) * n_j;
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );

    mesh_postprocessing(mesh, sigma_p, sigma_n, use_quadric_update, parallel_grain_size);
}

void mesh_normal_unilateral_filtering_tasdizen(halfedge_mesh &mesh,
                                               bcg_scalar_t sigma_g,
                                               bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, bool use_quadric_update,
                                               size_t parallel_grain_size) {

    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered", VectorS<3>::Zero());

    mesh_curvature_taubin(mesh, 1, true, parallel_grain_size);

    auto gauss_curvature = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_mesh_curv_gauss");

    bcg_scalar_t sigma_g_squared = sigma_g * sigma_g;
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_normals_filtered[f] = face_area(mesh, f) * face_normal(mesh, f);
                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            bcg_scalar_t x = (gauss_curvature[mesh.get_from_vertex(fh)] +
                                              gauss_curvature[mesh.get_to_vertex(fh)]) / 2.0;
                            bcg_scalar_t weight = std::exp(-x * x / sigma_g_squared);
                            f_normals_filtered[f] += weight * face_area(mesh, ff) * face_normal(mesh, ff);
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );

    mesh_postprocessing(mesh, sigma_p, sigma_n, use_quadric_update, parallel_grain_size);
}

void mesh_normal_unilateral_filtering_centin(halfedge_mesh &mesh,
                                             bcg_scalar_t sigma_g,
                                             bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, bool use_quadric_update,
                                             size_t parallel_grain_size) {

    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered", VectorS<3>::Zero());

    mesh_curvature_taubin(mesh, 1, true, parallel_grain_size);

    auto max_curvature = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_mesh_curv_max");

    tbb::atomic<bcg_scalar_t> l_avg = 0;

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    l_avg = l_avg + mesh.get_length(e) / mesh.edges.size();
                }
            }
    );
    bcg_scalar_t sigma_g_square = sigma_g * sigma_g;
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_normals_filtered[f] = face_area(mesh, f) * face_normal(mesh, f);

                    bcg_scalar_t k_avg = 0;
                    int count = 0;
                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto v = mesh.get_to_vertex(fh);
                        k_avg += max_curvature[v];
                        ++count;
                    }
                    k_avg /= count;

                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            bcg_scalar_t x = k_avg * l_avg;
                            bcg_scalar_t weight = 1.0;
                            if (std::abs(x) >= sigma_g) {
                                bcg_scalar_t diff = x - sigma_g;
                                weight = sigma_g_square / (diff * diff + sigma_g_square);
                            }

                            f_normals_filtered[f] += weight * face_area(mesh, ff) * face_normal(mesh, ff);
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );

    mesh_postprocessing(mesh, sigma_p, sigma_n, use_quadric_update, parallel_grain_size);
}


void mesh_normal_unilateral_filtering_probabilistic_quadric(halfedge_mesh &mesh,
                                                            int iterations,
                                                            bcg_scalar_t sigma_p, bcg_scalar_t /*sigma_n*/,
                                                            size_t parallel_grain_size) {
    auto positions = mesh.vertices.get<VectorS<3>, 3>("v_position");
    auto quadrics = mesh.faces.get_or_add<quadric, 1>("f_quadric");
    auto quadrics_avg = mesh.faces.get_or_add<quadric, 1>("f_quadric_avg");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);

                    std::vector<VectorS<3>> V;
                    for (const auto vf : mesh.get_vertices(f)) {
                        V.push_back(positions[vf]);
                    }
                    quadrics[f].probabilistic_triangle_quadric(V[0], V[1], V[2], sigma_p);
/*                    quadrics[f].probabilistic_plane_quadric(face_center(mesh, f), face_normal(mesh, f), sigma_p, sigma_n);*/
                }
            }
    );


    for (int iters = 0; iters < iterations; ++iters) {
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto f = face_handle(i);
                        bcg_scalar_t weight_sum = face_area(mesh, f);
                        quadrics_avg[f] = quadrics[f] * weight_sum;
                        for (const auto hf : mesh.get_halfedges(f)) {
                            auto oh = mesh.get_opposite(hf);
                            if (!mesh.is_boundary(oh)) {
                                auto ff = mesh.get_face(oh);
                                bcg_scalar_t weight = face_area(mesh, ff);
                                quadrics_avg[f] += quadrics[ff] * weight;
                                weight_sum += weight;
                            }
                        }
                        quadrics_avg[f] /= weight_sum;
                    }
                }
        );
        std::cout << "filtering face normals iters: " << iters << "/" << iterations << std::endl;
    }

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);

                    quadric Q_total;
                    for (const auto fv : mesh.get_faces(v)) {
                        Q_total += quadrics_avg[fv];
                    }

                    if (mesh.is_boundary(v)) {
                        VectorS<3> delta = Q_total.minimizer() - positions[v];
                        positions[v] += Q_total.A() * delta;
                    } else {
                        positions[v] = Q_total.minimizer();
                    }
                }
            }
    );
    positions.set_dirty();
    mesh.faces.remove(quadrics);
    mesh.faces.remove(quadrics_avg);
    vertex_normals(mesh, vertex_normal_area_angle, parallel_grain_size);
}

void mesh_normal_bilateral_filtering_zheng(halfedge_mesh &mesh,
                                           bcg_scalar_t sigma_g,
                                           bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, bool use_quadric_update,
                                           size_t parallel_grain_size) {
    face_normals(mesh, parallel_grain_size);
    auto f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered");
    auto e_g = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_g");
    auto fd = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_face_distance_squared");

    tbb::atomic<bcg_scalar_t> fd_avg = 0;
    bcg_scalar_t two_sigma_g_squared = 2 * sigma_g * sigma_g;
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    fd[e] = 0;
                    if (!mesh.is_boundary(e)) {
                        auto f0 = mesh.get_face(e, 0);
                        auto f1 = mesh.get_face(e, 1);
                        bcg_scalar_t face_distance = (face_center(mesh, f0) - face_center(mesh, f1)).norm();
                        fd_avg = fd_avg + face_distance;
                        fd[e] = face_distance * face_distance;
                        bcg_scalar_t x = (face_normal(mesh, f0) - face_normal(mesh, f1)).norm();
                        e_g[e] = std::exp(-x * x / two_sigma_g_squared);
                    }
                }
            }
    );

    fd_avg = fd_avg / mesh.edges.size();
    bcg_scalar_t two_fd_avg_squared = 2.0 * fd_avg * fd_avg;
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_normals_filtered[f] = face_normal(mesh, f) * face_area(mesh, f);

                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            auto e = mesh.get_edge(fh);
                            bcg_scalar_t weight = e_g[e] * std::exp(-fd[e] / two_fd_avg_squared);
                            f_normals_filtered[f] += weight * face_area(mesh, ff) * face_normal(mesh, ff);
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );
    e_g.set_dirty();
    mesh_postprocessing(mesh, sigma_p, sigma_n, use_quadric_update, parallel_grain_size);
}

void mesh_normal_bilateral_filtering_zhang(halfedge_mesh &mesh,
                                           bcg_scalar_t sigma_g,
                                           bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, bool use_quadric_update,
                                           size_t parallel_grain_size) {

    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered", VectorS<3>::Zero());
    auto e_g = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_g");
    auto fd = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_face_distance_squared");

    tbb::atomic<bcg_scalar_t> fd_avg = 0;

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    VectorS<3> n_i = face_normal(mesh, f);
                    f_normals_filtered[f] = n_i * face_area(mesh, f);

                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            VectorS<3> n_j = face_normal(mesh, ff);
                            bcg_scalar_t x = acos(1.0 - (n_i - n_j).squaredNorm() / 2.0);
                            if (x < sigma_g) {
                                f_normals_filtered[f] += n_j * face_area(mesh, ff);
                            }
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );
    bcg_scalar_t two_sigma_g_squared = 2 * sigma_g * sigma_g;
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    fd[e] = 0;
                    if (!mesh.is_boundary(e)) {
                        auto f0 = mesh.get_face(e, 0);
                        auto f1 = mesh.get_face(e, 1);
                        bcg_scalar_t face_distance = (face_center(mesh, f0) - face_center(mesh, f1)).norm();
                        fd_avg = fd_avg + face_distance;
                        fd[e] = face_distance * face_distance;
                        bcg_scalar_t x = (f_normals_filtered[f0] - f_normals_filtered[f1]).norm();
                        e_g[e] = std::exp(-x * x / two_sigma_g_squared);
                    }
                }
            }
    );

    fd_avg = fd_avg / mesh.edges.size();
    bcg_scalar_t two_fd_avg_squared = 2.0 * fd_avg * fd_avg;
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_normals_filtered[f] = face_normal(mesh, f) * face_area(mesh, f);

                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            auto e = mesh.get_edge(fh);
                            bcg_scalar_t weight = e_g[e] * std::exp(-fd[e] / two_fd_avg_squared);
                            f_normals_filtered[f] += weight * face_area(mesh, ff) * face_normal(mesh, ff);
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );
    e_g.set_dirty();
    mesh_postprocessing(mesh, sigma_p, sigma_n, use_quadric_update, parallel_grain_size);
}

void mesh_normal_bilateral_filtering_yadav(halfedge_mesh &mesh,
                                           bcg_scalar_t sigma_g,
                                           bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, bool use_quadric_update,
                                           size_t parallel_grain_size) {
    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered", VectorS<3>::Zero());
    auto e_g = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_g");
    auto fd = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_face_distance");

    tbb::atomic<bcg_scalar_t> fd_avg = 0;
    bcg_scalar_t sigma_g_squared = sigma_g * sigma_g;
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    fd[e] = 0;
                    e_g[e] = 0;
                    if (!mesh.is_boundary(e)) {
                        auto f0 = mesh.get_face(e, 0);
                        auto f1 = mesh.get_face(e, 1);
                        bcg_scalar_t face_distance = (face_center(mesh, f0) - face_center(mesh, f1)).norm();
                        fd_avg = fd_avg + face_distance;
                        fd[e] = face_distance * face_distance;
                        bcg_scalar_t x = (face_normal(mesh, f0) - face_normal(mesh, f1)).norm();
                        if (std::abs(x) < sigma_g) {
                            e_g[e] = 0.5 * (1.0 - (x * x / sigma_g_squared));
                        }
                    }
                }
            }
    );

    fd_avg = fd_avg / mesh.edges.size();
    bcg_scalar_t two_fd_avg_squared = 2 * fd_avg * fd_avg;
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_normals_filtered[f] = face_area(mesh, f) * face_normal(mesh, f);

                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            auto e = mesh.get_edge(fh);
                            bcg_scalar_t weight = e_g[e] * std::exp(-fd[e] / two_fd_avg_squared);
                            f_normals_filtered[f] += weight * face_area(mesh, ff) * face_normal(mesh, ff);
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );

    e_g.set_dirty();

    mesh_postprocessing(mesh, sigma_p, sigma_n, use_quadric_update, parallel_grain_size);
}

}