//
// Created by alex on 17.02.21.
//

#ifndef BCG_GRAPHICS_BCG_MESH_NORMAL_FILTERING_ROBUST_STATISTICS_H
#define BCG_GRAPHICS_BCG_MESH_NORMAL_FILTERING_ROBUST_STATISTICS_H

#include "bcg_mesh.h"

namespace bcg {

enum class NormalFilteringType {
    unilateral_belyaev_ohtake,
    unilateral_yagou_mean,
    unilateral_yagou_median,
    unilateral_yadav,
    unilateral_shen,
    unilateral_tasdizen,
    unilateral_centin,
    unilateral_probabilistic_quadric,
    bilateral_zheng,
    bilateral_zhang,
    bilateral_yadav,
    __last__
};

std::vector<std::string> normal_filtering_names();

void mesh_normal_unilateral_filtering_belyaev_ohtake(halfedge_mesh &mesh,
                                                     bcg_scalar_t sigma_g,
                                                     bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                                     bool use_quadric_update,
                                                     size_t parallel_grain_size = 1024);

void mesh_normal_unilateral_filtering_yagou_mean(halfedge_mesh &mesh,
                                                 bcg_scalar_t sigma_g,
                                                 bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, bool use_quadric_update,
                                                 size_t parallel_grain_size = 1024);

void mesh_normal_unilateral_filtering_yagou_median(halfedge_mesh &mesh,
                                                   bcg_scalar_t sigma_g,
                                                   bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, bool use_quadric_update,
                                                   size_t parallel_grain_size = 1024);

void mesh_normal_unilateral_filtering_yadav(halfedge_mesh &mesh,
                                            bcg_scalar_t sigma_g,
                                            bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, bool use_quadric_update,
                                            size_t parallel_grain_size = 1024);

void mesh_normal_unilateral_filtering_shen(halfedge_mesh &mesh,
                                           bcg_scalar_t sigma_g,
                                           bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, bool use_quadric_update,
                                           size_t parallel_grain_size = 1024);

void mesh_normal_unilateral_filtering_tasdizen(halfedge_mesh &mesh,
                                               bcg_scalar_t sigma_g,
                                               bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, bool use_quadric_update,
                                               size_t parallel_grain_size = 1024);

void mesh_normal_unilateral_filtering_centin(halfedge_mesh &mesh,
                                             bcg_scalar_t sigma_g,
                                             bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, bool use_quadric_update,
                                             size_t parallel_grain_size = 1024);

void mesh_normal_unilateral_filtering_probabilistic_quadric(halfedge_mesh &mesh,
                                                            int iterations,
                                                            bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                                            size_t parallel_grain_size = 1024);

void mesh_normal_bilateral_filtering_zheng(halfedge_mesh &mesh,
                                           bcg_scalar_t sigma_g,
                                           bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, bool use_quadric_update,
                                           size_t parallel_grain_size = 1024);

void mesh_normal_bilateral_filtering_zhang(halfedge_mesh &mesh,
                                           bcg_scalar_t sigma_g,
                                           bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, bool use_quadric_update,
                                           size_t parallel_grain_size = 1024);

void mesh_normal_bilateral_filtering_yadav(halfedge_mesh &mesh,
                                           bcg_scalar_t sigma_g,
                                           bcg_scalar_t sigma_p, bcg_scalar_t sigma_n, bool use_quadric_update,
                                           size_t parallel_grain_size = 1024);

//m-estimators

void mesh_denoising_vertex_update_naive(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

void mesh_denoising_vertex_update_plane_quadric(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

void mesh_denoising_vertex_update_probabilistic_plane_quadric_global_sigma(halfedge_mesh &mesh, bcg_scalar_t sigma_p,
                                                                           bcg_scalar_t sigma_n,
                                                                           size_t parallel_grain_size = 1024);

void mesh_denoising_vertex_update_probabilistic_plane_quadric_local_sigma(halfedge_mesh &mesh,
                                                                          property<bcg_scalar_t, 1> sigma_p,
                                                                          property<bcg_scalar_t, 1> sigma_n,
                                                                          size_t parallel_grain_size = 1024);

void
mesh_denoising_vertex_update_probabilistic_plane_quadric_global_sigma(halfedge_mesh &mesh, const MatrixS<3, 3> &sigma_p,
                                                                      const MatrixS<3, 3> &sigma_n,
                                                                      size_t parallel_grain_size = 1024);

void
mesh_denoising_vertex_update_probabilistic_plane_quadric_local_sigma(halfedge_mesh &mesh,
                                                                      property<MatrixS<3, 3>, 1> sigma_p,
                                                                      property<MatrixS<3, 3>, 1> sigma_n,
                                                                      size_t parallel_grain_size = 1024);

void mesh_denoising_vertex_update_triangle_quadric(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

void mesh_denoising_vertex_update_probabilistic_triangle_quadric_global_sigma(halfedge_mesh &mesh, bcg_scalar_t sigma_p,
                                                                              size_t parallel_grain_size = 1024);

void mesh_denoising_vertex_update_probabilistic_triangle_quadric_local_sigma(halfedge_mesh &mesh,
                                                                             property<bcg_scalar_t, 1> sigma_p,
                                                                             size_t parallel_grain_size = 1024);

void mesh_denoising_vertex_update_probabilistic_triangle_quadric_global_sigma(halfedge_mesh &mesh,
                                                                              const MatrixS<3, 3> &sigma_p,
                                                                              size_t parallel_grain_size = 1024);

void mesh_denoising_vertex_update_probabilistic_triangle_quadric_local_sigma(halfedge_mesh &mesh,
                                                                             property<MatrixS<3, 3>, 1> sigma_p,
                                                                             size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_NORMAL_FILTERING_ROBUST_STATISTICS_H
