//
// Created by alex on 29.03.21.
//

#ifndef BCG_GRAPHICS_BCG_POINT_CLOUD_EXTREMAL_VERTEX_VALUES_H
#define BCG_GRAPHICS_BCG_POINT_CLOUD_EXTREMAL_VERTEX_VALUES_H

#include "bcg_point_cloud.h"
#include "kdtree/bcg_kdtree.h"

namespace bcg {

enum class VertexExtremalType{
    minimum,
    maximum,
    extremal,
    __last__
};

std::vector<std::string> vertex_extremal_type_names();

void point_cloud_extremal_vertex_values_radius(vertex_container *vertices, property<bcg_scalar_t, 1> property,
                                               const kdtree_property<bcg_scalar_t> &index, bcg_scalar_t radius, size_t parallel_grain_size = 1024);

void point_cloud_extremal_vertex_values_knn(vertex_container *vertices, property<bcg_scalar_t, 1> property,
                                            const kdtree_property<bcg_scalar_t> &index, int num_closest, size_t parallel_grain_size = 1024);

void point_cloud_minimal_vertex_values_radius(vertex_container *vertices, property<bcg_scalar_t, 1> property,
                                               const kdtree_property<bcg_scalar_t> &index, bcg_scalar_t radius, size_t parallel_grain_size = 1024);

void point_cloud_minimal_vertex_values_knn(vertex_container *vertices, property<bcg_scalar_t, 1> property,
                                            const kdtree_property<bcg_scalar_t> &index, int num_closest, size_t parallel_grain_size = 1024);

void point_cloud_maximal_vertex_values_radius(vertex_container *vertices, property<bcg_scalar_t, 1> property,
                                               const kdtree_property<bcg_scalar_t> &index, bcg_scalar_t radius, size_t parallel_grain_size = 1024);

void point_cloud_maximal_vertex_values_knn(vertex_container *vertices, property<bcg_scalar_t, 1> property,
                                            const kdtree_property<bcg_scalar_t> &index, int num_closest, size_t parallel_grain_size = 1024);

}
#endif //BCG_GRAPHICS_BCG_POINT_CLOUD_EXTREMAL_VERTEX_VALUES_H
