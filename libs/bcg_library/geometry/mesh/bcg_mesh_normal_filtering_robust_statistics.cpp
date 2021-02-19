//
// Created by alex on 17.02.21.
//

#include "bcg_mesh_normal_filtering_robust_statistics.h"
#include "bcg_mesh_face_centers.h"
#include "bcg_mesh_face_normals.h"
#include "bcg_mesh_face_areas.h"
#include "bcg_mesh_vertex_normals.h"
#include "bcg_property_map_eigen.h"
#include "geometry/quadric/bcg_quadric.h"
#include "tbb/tbb.h"

namespace bcg {

void mesh_normal_filtering_robust_statistics(halfedge_mesh &mesh,
                                             property<bcg_scalar_t, 1> g,
                                             property<bcg_scalar_t, 1> f,
                                             bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                             size_t parallel_grain_size) {
    auto positions = mesh.vertices.get<VectorS<3>, 3>("v_position");
    auto face_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    auto filtered_face_normals = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal_filtered");
    auto f_centers = mesh.faces.get_or_add<VectorS<3>, 3>("f_position");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    filtered_face_normals[f].setZero();
                    for (const auto &fh : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(fh);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            bcg_scalar_t weight = g[mesh.get_edge(fh)] * f[mesh.get_edge(fh)];
                            filtered_face_normals[f] += weight * face_normals[ff];
                        }
                    }
                    filtered_face_normals[f].normalize();
                }
            }
    );

    Map(face_normals) = MapConst(filtered_face_normals);

    vertex_normals(mesh, vertex_normal_area_angle, parallel_grain_size);

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);

                    quadric Q_total;
                    for (const auto &fv : mesh.get_faces(v)) {
                        MatrixS<3, 3> A = face_normals[fv] * face_normals[fv].transpose();
                        Quadric.block<3, 3>(0, 0) += A;
                        Quadric.block<1, 3>(2, 0) = A * face_centers[fv];
                        Quadric.block<3, 1>(0, 2) = Quadric.block<1, 3>(3, 0).transpose();
                        Quadric(3, 3) = face_centers[fv].transpose() * A * face_centers[fv];
                        quadric Q;
                        Q.probabilistic_plane_quadric(face_centers[fc], face_normals[fv], sigma_p, sigma_n);
                        Q_total += Q;
                    }
                    positions[v] = Q_total.minimizer();
                }
            }
    );

    face_normals(mesh, parallel_grain_size);
    vertex_normals(mesh, vertex_normal_area_angle, parallel_grain_size);
    positions.set_dirty();
    mesh.faces.remove(filtered_face_normals);
}

void mesh_normal_filtering_belyaev_ohtake(halfedge_mesh &mesh,
                                          bcg_scalar_t sigma_g,
                                          bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                          size_t parallel_grain_size) {
    auto f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    auto f_centers = mesh.faces.get_or_add<VectorS<3>, 3>("f_position");
    auto g = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_g");
    auto f = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_f");
    Map(f).setOnes();

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    g[e] = 0;
                    if (!mesh.is_boundary(e)) {
                        auto f0 = mesh.get_face(e, 0);
                        auto f1 = mesh.get_face(e, 1);
                        f_centers[f0] = face_center(mesh, f0);
                        f_centers[f1] = face_center(mesh, f1);
                        bcg_scalar_t x = acos(1.0 - (f_normals[f0] - f_normals[f1]).squaredNorm() / 2.0) /
                                         (f_centers[f0] - f_centers[f1]).norm();
                        g[e] = std::exp(-x * x / (sigma_g * sigma_g));
                    }
                }
            }
    );


    mesh_normal_filtering_robust_statistics(mesh, g, f, sigma_p, sigma_n, parallel_grain_size);
}

void mesh_normal_filtering_yagou_mean(halfedge_mesh &mesh,
                                      bcg_scalar_t sigma_g,
                                      bcg_scalar_t sigma_p, bcg_scalar_t sigma_n
                                      size_t parallel_grain_size){
    auto g = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_g");
    auto f = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_normal_filtering_f");
    Map(g).setOnes();

    face_areas(mesh, parallel_grain_size);
    auto f_areas = mesh.faces.get_or_add<bcg_scalar_t, 1>("f_area");
    Map(f) = MapConst(f_areas);

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    g[e] = 0;
                    if (!mesh.is_boundary(e)) {
                        auto f0 = mesh.get_face(e, 0);
                        auto f1 = mesh.get_face(e, 1);
                        f_centers[f0] = face_center(mesh, f0);
                        f_centers[f1] = face_center(mesh, f1);
                        bcg_scalar_t x = acos(1.0 - (f_normals[f0] - f_normals[f1]).squaredNorm() / 2.0) /
                                         (f_centers[f0] - f_centers[f1]).norm();
                        g[e] = std::exp(-x * x / (sigma_g * sigma_g));
                    }
                }
            }
    );


    mesh_normal_filtering_robust_statistics(mesh, g, f, sigma_p, sigma_n, parallel_grain_size);
}

void mesh_normal_filtering_yagou_median(halfedge_mesh &mesh,
                                        bcg_scalar_t sigma_g,
                                        bcg_scalar_t sigma_p, bcg_scalar_t sigma_n
                                        size_t parallel_grain_size);

void mesh_normal_filtering_huber(halfedge_mesh &mesh,
                                 bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                 size_t parallel_grain_size);

void mesh_normal_filtering_yadav(halfedge_mesh &mesh,
                                 bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                 size_t parallel_grain_size);

void mesh_normal_filtering_shen(halfedge_mesh &mesh,
                                bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                size_t parallel_grain_size);

void mesh_normal_filtering_tasdizen(halfedge_mesh &mesh,
                                    bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                    size_t parallel_grain_size);

void mesh_normal_filtering_centin(halfedge_mesh &mesh,
                                  bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                  size_t parallel_grain_size);

void mesh_normal_bilateral_filtering_zheng(halfedge_mesh &mesh,
                                           bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                           size_t parallel_grain_size);

void mesh_normal_bilateral_filtering_zhang(halfedge_mesh &mesh,
                                           bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                           size_t parallel_grain_size);

void mesh_normal_bilateral_filtering_yadav(halfedge_mesh &mesh,
                                           bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                           size_t parallel_grain_size);

}