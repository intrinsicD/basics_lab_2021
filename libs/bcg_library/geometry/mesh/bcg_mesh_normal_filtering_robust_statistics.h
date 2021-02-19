//
// Created by alex on 17.02.21.
//

#ifndef BCG_GRAPHICS_BCG_MESH_NORMAL_FILTERING_ROBUST_STATISTICS_H
#define BCG_GRAPHICS_BCG_MESH_NORMAL_FILTERING_ROBUST_STATISTICS_H

#include "bcg_mesh.h"

namespace bcg {

void mesh_normal_filtering_robust_statistics(halfedge_mesh &mesh,
                                             property<bcg_scalar_t, 1> g,
                                             property<bcg_scalar_t, 1> f,
                                             bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                             size_t parallel_grain_size = 1024);

void mesh_normal_filtering_belyaev_ohtake(halfedge_mesh &mesh,
                                          bcg_scalar_t sigma_g,
                                          bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                          size_t parallel_grain_size = 1024);

void mesh_normal_filtering_yagou_mean(halfedge_mesh &mesh,
                                          bcg_scalar_t sigma_g,
                                          bcg_scalar_t sigma_p, bcg_scalar_t sigma_n
                                          size_t parallel_grain_size = 1024);

void mesh_normal_filtering_yagou_median(halfedge_mesh &mesh,
                                      bcg_scalar_t sigma_g,
                                      bcg_scalar_t sigma_p, bcg_scalar_t sigma_n
                                      size_t parallel_grain_size = 1024);

void mesh_normal_filtering_huber(halfedge_mesh &mesh,
                                          bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                          size_t parallel_grain_size = 1024);

void mesh_normal_filtering_yadav(halfedge_mesh &mesh,
                                          bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                          size_t parallel_grain_size = 1024);

void mesh_normal_filtering_shen(halfedge_mesh &mesh,
                                          bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                          size_t parallel_grain_size = 1024);

void mesh_normal_filtering_tasdizen(halfedge_mesh &mesh,
                                          bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                          size_t parallel_grain_size = 1024);

void mesh_normal_filtering_centin(halfedge_mesh &mesh,
                                          bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                          size_t parallel_grain_size = 1024);

void mesh_normal_bilateral_filtering_zheng(halfedge_mesh &mesh,
                                          bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                          size_t parallel_grain_size = 1024);

void mesh_normal_bilateral_filtering_zhang(halfedge_mesh &mesh,
                                           bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                           size_t parallel_grain_size = 1024);

void mesh_normal_bilateral_filtering_yadav(halfedge_mesh &mesh,
                                           bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                           size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_NORMAL_FILTERING_ROBUST_STATISTICS_H
