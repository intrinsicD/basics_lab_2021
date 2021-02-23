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

std::vector<std::string> normal_filtering_names(){
    std::vector<std::string> names(static_cast<int>(NormalFilteringType::__last__));
    names[static_cast<int>(NormalFilteringType::unilateral_belyaev_ohtake)] = "unilateral_belyaev_ohtake";
    names[static_cast<int>(NormalFilteringType::unilateral_yagou_mean)] = "unilateral_yagou_mean";
    names[static_cast<int>(NormalFilteringType::unilateral_yagou_median)] = "unilateral_yagou_median";
    names[static_cast<int>(NormalFilteringType::unilateral_yadav)] = "unilateral_yadav";
    names[static_cast<int>(NormalFilteringType::unilateral_shen)] = "unilateral_shen";
    names[static_cast<int>(NormalFilteringType::unilateral_tasdizen)] = "unilateral_tasdizen";
    names[static_cast<int>(NormalFilteringType::unilateral_centin)] = "unilateral_centin";
    names[static_cast<int>(NormalFilteringType::bilateral_zheng)] = "bilateral_zheng";
    names[static_cast<int>(NormalFilteringType::bilateral_zhang)] = "bilateral_zhang";
    names[static_cast<int>(NormalFilteringType::bilateral_yadav)] = "bilateral_yadav";
    return names;
}

void mesh_postprocessing(halfedge_mesh &mesh, bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, size_t parallel_grain_size){
    auto positions = mesh.vertices.get<VectorS<3>, 3>("v_position");
    auto f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered");

    Map(f_normals) = MapConst(f_normals_filtered);

    vertex_normals(mesh, vertex_normal_uniform, parallel_grain_size);

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);

                    quadric Q_total;
                    for (const auto &fv : mesh.get_faces(v)) {
                        quadric Q;
                        Q.probabilistic_plane_quadric(face_center(mesh, fv), f_normals[fv], sigma_p, sigma_n);
                        Q_total += Q;
                    }
                    positions[v] = Q_total.minimizer();
                }
            }
    );

    face_normals(mesh, parallel_grain_size);
    vertex_normals(mesh, vertex_normal_area_angle, parallel_grain_size);
    positions.set_dirty();
    mesh.faces.remove(f_normals_filtered);
}

void mesh_normal_unilateral_filtering_belyaev_ohtake(halfedge_mesh &mesh,
                                          bcg_scalar_t sigma_g,
                                          bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                          size_t parallel_grain_size) {
    auto f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered");
    auto e_g = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_g");
    auto e_f = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_f");

    if(!f_normals){
        face_normals(mesh, parallel_grain_size);
        f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    }
    Map(e_f).setOnes();
    Map(f_normals_filtered).setZero();

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    e_g[e] = 0;
                    if (!mesh.is_boundary(e)) {
                        auto f0 = mesh.get_face(e, 0);
                        auto f1 = mesh.get_face(e, 1);
                        bcg_scalar_t x = acos(1.0 - (f_normals[f0] - f_normals[f1]).squaredNorm() / 2.0) /
                                         (face_center(mesh, f0) - face_center(mesh, f1)).norm();
                        e_g[e] = std::exp(-x * x / (sigma_g * sigma_g));
                    }
                }
            }
    );

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_normals_filtered[f].setZero();
                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            auto e = mesh.get_edge(fh);
                            bcg_scalar_t weight = e_g[e] * e_f[e];
                            f_normals_filtered[f] += weight * f_normals[ff];
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );
    e_g.set_dirty();
    e_f.set_dirty();
    mesh_postprocessing(mesh, sigma_p, sigma_n, parallel_grain_size);
}

void mesh_normal_unilateral_filtering_yagou_mean(halfedge_mesh &mesh,
                                      bcg_scalar_t,
                                      bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                      size_t parallel_grain_size) {
    auto f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered");
    auto e_g = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_g");
    if(!f_normals){
        face_normals(mesh, parallel_grain_size);
        f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    }
    face_areas(mesh, parallel_grain_size);
    auto f_areas = mesh.faces.get_or_add<bcg_scalar_t, 1>("f_area");
    Map(e_g).setOnes();

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_normals_filtered[f].setZero();
                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            auto e = mesh.get_edge(fh);
                            bcg_scalar_t weight = e_g[e];
                            f_normals_filtered[f] += weight * f_areas[ff] * f_normals[ff];
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );
    e_g.set_dirty();
    mesh_postprocessing(mesh, sigma_p, sigma_n, parallel_grain_size);
}

void mesh_normal_unilateral_filtering_yagou_median(halfedge_mesh &mesh,
                                        bcg_scalar_t sigma_g,
                                        bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                        size_t parallel_grain_size){
    auto f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered");
    auto e_g = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_g");
    if(!f_normals){
        face_normals(mesh, parallel_grain_size);
        f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    }
    face_areas(mesh, parallel_grain_size);
    auto f_areas = mesh.faces.get_or_add<bcg_scalar_t, 1>("f_area");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    e_g[e] = 0;
                    if (!mesh.is_boundary(e)) {
                        auto f0 = mesh.get_face(e, 0);
                        auto f1 = mesh.get_face(e, 1);
                        bcg_scalar_t x = (f_normals[f0] - f_normals[f1]).norm();
                        if(x < sigma_g){
                            if(x > scalar_eps){
                                e_g[e] = 1.0 / std::abs(x);
                            }else{
                                e_g[e] = 1.0;
                            }
                        }
                    }
                }
            }
    );

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_normals_filtered[f].setZero();
                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            auto e = mesh.get_edge(fh);
                            bcg_scalar_t weight = e_g[e];
                            f_normals_filtered[f] += weight * f_areas[ff] * f_normals[ff];
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );
    e_g.set_dirty();
    mesh_postprocessing(mesh, sigma_p, sigma_n, parallel_grain_size);
}

void mesh_normal_unilateral_filtering_yadav(halfedge_mesh &mesh,
                                 bcg_scalar_t sigma_g,
                                 bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                 size_t parallel_grain_size){
    auto f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered");
    auto e_g = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_g");
    auto e_f = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_f");
    Map(e_f).setOnes();
    if(!f_normals){
        face_normals(mesh, parallel_grain_size);
        f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    }
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    e_g[e] = 0.1;
                    if (!mesh.is_boundary(e)) {
                        auto f0 = mesh.get_face(e, 0);
                        auto f1 = mesh.get_face(e, 1);
                        bcg_scalar_t x = acos(1.0 - (f_normals[f0] - f_normals[f1]).squaredNorm() / 2.0);
                        if(x < sigma_g){
                            e_g[e] = 1.0;
                        }
                    }
                }
            }
    );

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_normals_filtered[f].setZero();
                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            auto e = mesh.get_edge(fh);
                            bcg_scalar_t weight = e_g[e] * e_f[e];
                            f_normals_filtered[f] += weight * f_normals[ff];
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );
    e_g.set_dirty();
    e_f.set_dirty();
    mesh_postprocessing(mesh, sigma_p, sigma_n, parallel_grain_size);
}

void mesh_normal_unilateral_filtering_shen(halfedge_mesh &mesh,
                                bcg_scalar_t sigma_g,
                                bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                size_t parallel_grain_size){
    auto f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered");
    auto e_g = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_g");
    auto e_f = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_f");
    Map(e_f).setOnes();
    if(!f_normals){
        face_normals(mesh, parallel_grain_size);
        f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    }
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_normals_filtered[f].setZero();
                    std::vector<VectorS<3>> normals;
                    normals.push_back(f_normals[f]);
                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            normals.push_back(f_normals[ff]);
                        }
                    }
                    VectorS<3> median = vector_median_filter_directional(normals);

                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            auto e = mesh.get_edge(fh);
                            bcg_scalar_t x = (median - f_normals[ff]).norm();
                            e_g[e] = std::exp(-x * x / (sigma_g * sigma_g));
                            bcg_scalar_t weight = e_g[e] * e_f[e];
                            f_normals_filtered[f] += weight * f_normals[ff];
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );
    e_g.set_dirty();
    e_f.set_dirty();
    mesh_postprocessing(mesh, sigma_p, sigma_n, parallel_grain_size);
}

void mesh_normal_unilateral_filtering_tasdizen(halfedge_mesh &mesh,
                                    bcg_scalar_t sigma_g,
                                    bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                    size_t parallel_grain_size){
    auto f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered");
    auto e_g = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_g");
    auto e_f = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_f");
    if(!f_normals){
        face_normals(mesh, parallel_grain_size);
        f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    }
    mesh_curvature_taubin(mesh, 1, true, parallel_grain_size);

    auto gauss_curvature = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_mesh_curv_gauss");

    Map(e_f).setOnes();

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    if (!mesh.is_boundary(e)) {
                        auto f0 = mesh.get_face(e, 0);
                        auto f1 = mesh.get_face(e, 1);
                        bcg_scalar_t x = (gauss_curvature[mesh.get_vertex(e, 0)] + gauss_curvature[mesh.get_vertex(e, 1)]) / 2.0;
                        e_g[e] = std::exp(-x * x / (sigma_g * sigma_g));
                    }
                }
            }
    );

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_normals_filtered[f].setZero();
                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            auto e = mesh.get_edge(fh);
                            bcg_scalar_t weight = e_g[e] * e_f[e];
                            f_normals_filtered[f] += weight * f_normals[ff];
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );
    e_g.set_dirty();
    e_f.set_dirty();
    mesh_postprocessing(mesh, sigma_p, sigma_n, parallel_grain_size);
}

void mesh_normal_unilateral_filtering_centin(halfedge_mesh &mesh,
                                  bcg_scalar_t sigma_g,
                                  bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                  size_t parallel_grain_size){
    auto f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered");
    auto e_g = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_g");
    auto e_f = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_f");
    auto l = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_length");
    if(!f_normals){
        face_normals(mesh, parallel_grain_size);
        f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    }
    mesh_curvature_taubin(mesh, 1, true, parallel_grain_size);

    auto max_curvature = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_mesh_curv_max");

    Map(e_f).setOnes();

    tbb::atomic<bcg_scalar_t> l_avg = 0;

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    l[e] = mesh.get_length(e);
                    l_avg = l_avg + l[e] / mesh.edges.size();
                }
            }
    );

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_normals_filtered[f].setZero();

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
                            auto e = mesh.get_edge(fh);
                            bcg_scalar_t x = k_avg * l_avg;
                            if(std::abs(x) < sigma_g){
                                e_g[e] = 1.0;
                            }else{
                                bcg_scalar_t diff = x - sigma_g;
                                bcg_scalar_t sigma_square = sigma_g * sigma_g;
                                e_g[e] = sigma_square / (diff * diff + sigma_square);
                            }
                            bcg_scalar_t weight = e_g[e] * e_f[e];
                            f_normals_filtered[f] += weight * f_normals[ff];
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );
    e_g.set_dirty();
    e_f.set_dirty();
    mesh_postprocessing(mesh, sigma_p, sigma_n, parallel_grain_size);
}

void mesh_normal_bilateral_filtering_zheng(halfedge_mesh &mesh,
                                           bcg_scalar_t sigma_g,
                                           bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                           size_t parallel_grain_size){
    auto f_normals = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal");
    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered");
    auto e_g = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_g");
    auto e_f = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_f");
    auto fd = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_face_distance");
    if(!f_normals){
        face_normals(mesh, parallel_grain_size);
        f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    }
    tbb::atomic<bcg_scalar_t> fd_avg = 0;

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    fd[e] = 0;
                    if (!mesh.is_boundary(e)) {
                        auto f0 = mesh.get_face(e, 0);
                        auto f1 = mesh.get_face(e, 1);
                        fd[e] = (face_center(mesh, f0) - face_center(mesh, f1)).norm();
                        fd_avg = fd_avg + fd[e];
                        bcg_scalar_t x = (f_normals[f0] - f_normals[f1]).norm();
                        e_g[e] = std::exp(- x * x / (2 * sigma_g * sigma_g));
                    }
                }
            }
    );

    fd_avg = fd_avg / mesh.edges.size();

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_normals_filtered[f].setZero();

                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            auto e = mesh.get_edge(fh);
                            e_f[e] = std::exp(-fd[e] * fd[e] / (2 * fd_avg * fd_avg));
                            bcg_scalar_t weight = e_g[e] * e_f[e];
                            f_normals_filtered[f] += weight * f_normals[ff];
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );
    e_g.set_dirty();
    e_f.set_dirty();
    mesh_postprocessing(mesh, sigma_p, sigma_n, parallel_grain_size);
}

void mesh_normal_bilateral_filtering_zhang(halfedge_mesh &mesh,
                                           bcg_scalar_t sigma_g,
                                           bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                           size_t parallel_grain_size){
    auto f_normals = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal");
    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered");
    auto e_g = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_g");
    auto e_f = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_f");
    auto fd = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_face_distance");
    if(!f_normals){
        face_normals(mesh, parallel_grain_size);
        f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    }
    tbb::atomic<bcg_scalar_t> fd_avg = 0;

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_normals_filtered[f] = f_normals[f];

                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            auto e = mesh.get_edge(fh);

                            bcg_scalar_t x = acos(1.0 - (f_normals[ff] - f_normals[f]).squaredNorm() / 2.0);
                            if(x < sigma_g){
                                f_normals_filtered[f] += f_normals[ff];
                            }
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    fd[e] = 0;
                    if (!mesh.is_boundary(e)) {
                        auto f0 = mesh.get_face(e, 0);
                        auto f1 = mesh.get_face(e, 1);
                        fd[e] = (face_center(mesh, f0) - face_center(mesh, f1)).norm();
                        fd_avg = fd_avg + fd[e];
                        bcg_scalar_t x = (f_normals_filtered[f0] - f_normals_filtered[f1]).norm();
                        e_g[e] = std::exp(- x * x / (2 * sigma_g * sigma_g));
                    }
                }
            }
    );

    fd_avg = fd_avg / mesh.edges.size();

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_normals_filtered[f].setZero();

                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            auto e = mesh.get_edge(fh);
                            e_f[e] = std::exp(-fd[e] * fd[e] / (2 * fd_avg * fd_avg));
                            bcg_scalar_t weight = e_g[e] * e_f[e];
                            f_normals_filtered[f] += weight * f_normals[ff];
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );
    e_g.set_dirty();
    e_f.set_dirty();
    mesh_postprocessing(mesh, sigma_p, sigma_n, parallel_grain_size);
}

void mesh_normal_bilateral_filtering_yadav(halfedge_mesh &mesh,
                                           bcg_scalar_t sigma_g,
                                           bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                           size_t parallel_grain_size){
    auto f_normals = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal");
    auto f_normals_filtered = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered");
    auto e_g = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_g");
    auto e_f = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_f");
    auto fd = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_face_distance");
    if(!f_normals){
        face_normals(mesh, parallel_grain_size);
        f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    }
    tbb::atomic<bcg_scalar_t> fd_avg = 0;

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
                        fd[e] = (face_center(mesh, f0) - face_center(mesh, f1)).norm();
                        fd_avg = fd_avg + fd[e];
                        bcg_scalar_t x = (f_normals[f0] - f_normals[f1]).norm();
                        if(std::abs(x) < sigma_g){
                            e_g[e] = 0.5 * (1.0 - (x * x / sigma_g * sigma_g));
                        }
                    }
                }
            }
    );

    fd_avg = fd_avg / mesh.edges.size();

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_normals_filtered[f].setZero();

                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            auto e = mesh.get_edge(fh);
                            e_f[e] = std::exp(-fd[e] * fd[e] / (2 * fd_avg * fd_avg));
                            bcg_scalar_t weight = e_g[e] * e_f[e];
                            f_normals_filtered[f] += weight * f_normals[ff];
                        }
                    }
                    f_normals_filtered[f].normalize();
                }
            }
    );

    e_g.set_dirty();
    e_f.set_dirty();

    mesh_postprocessing(mesh, sigma_p, sigma_n, parallel_grain_size);
}

}