//
// Created by alex on 01.03.21.
//

#ifndef BCG_GRAPHICS_BCG_POINT_CLOUD_NORMAL_FILTERING_H
#define BCG_GRAPHICS_BCG_POINT_CLOUD_NORMAL_FILTERING_H

#include "geometry/point_cloud/bcg_point_cloud.h"
#include "kdtree/bcg_kdtree.h"

namespace bcg{

std::vector<std::string> search_type_names();

enum class SearchType{
    knn,
    radius
};

struct pc_search_params{
    SearchType search_type;
    bcg_scalar_t radius;
    int num_closest;
};

struct pc_nf_parameters{
    bcg_scalar_t sigma_f;
    bcg_scalar_t sigma_g;
    int iterations;
    pc_search_params search_params;
};

void point_cloud_normal_filtering(vertex_container *vertices, kdtree_property<bcg_scalar_t> &index, const pc_nf_parameters &params, size_t parallel_grain_size = 1024);

}
#endif //BCG_GRAPHICS_BCG_POINT_CLOUD_NORMAL_FILTERING_H
