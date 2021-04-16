//
// Created by alex on 03.12.20.
//

#ifndef BCG_GRAPHICS_BCG_POINT_CLOUD_VERTEX_QUADRIC_H
#define BCG_GRAPHICS_BCG_POINT_CLOUD_VERTEX_QUADRIC_H

#include "bcg_point_cloud.h"
#include "kdtree/bcg_kdtree.h"
#include "math/bcg_probabilistic_quadric.h"

namespace bcg {

using quadric = pq::quadric<pq::math<bcg_scalar_t, VectorS<3>, VectorS<3>, MatrixS<3, 3>>>;

enum class PointCloudQuadricTypes{
    point,
    plane,
    isotropic,
    anisotropic,
    __last__
};

std::vector<std::string> quadric_names();

quadric point_quadric(vertex_handle v, property<VectorS<3>, 3> positions, const neighbors_query &result,
                      property<VectorS<3>, 3> normals = {});

quadric plane_quadric(vertex_handle v, property<VectorS<3>, 3> positions, const neighbors_query &result,
                      property<VectorS<3>, 3> normals);

quadric
probabilistic_plane_quadric_isotropic(vertex_handle v, property<VectorS<3>, 3> positions, const neighbors_query &result,
                                      property<VectorS<3>, 3> normals);

quadric probabilistic_plane_quadric_anisotropic(vertex_handle v, property<VectorS<3>, 3> positions,
                                                const neighbors_query &result,
                                                property<VectorS<3>, 3> normals);

void point_cloud_vertex_quadric_knn(vertex_container *vertices,
                                    std::function<quadric(vertex_handle, property<VectorS<3>, 3>,
                                                          const neighbors_query &,
                                                          property<VectorS<3>, 3>)> method,
                                    const kdtree_property<bcg_scalar_t> &index, int num_closest,
                                    size_t parallel_grain_size = 1024);

void point_cloud_vertex_quadric_radius(vertex_container *vertices,
                                       std::function<quadric(vertex_handle, property<VectorS<3>, 3>,
                                                             const neighbors_query &,
                                                             property<VectorS<3>, 3>)> method,
                                       const kdtree_property<bcg_scalar_t> &index, bcg_scalar_t radius,
                                       size_t parallel_grain_size = 1024);

void point_cloud_vertex_quadric_collect_neighbors(vertex_container *vertices, size_t parallel_grain_size = 1024);

}


#endif //BCG_GRAPHICS_BCG_POINT_CLOUD_VERTEX_QUADRIC_H
