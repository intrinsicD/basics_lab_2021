//
// Created by alex on 28.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_STATISTICS_H
#define BCG_GRAPHICS_BCG_MESH_STATISTICS_H

#include "bcg_mesh.h"

namespace bcg{

struct mesh_stats{
    struct {
        bcg_scalar_t min_valence;
        bcg_scalar_t avg_valence;
        bcg_scalar_t max_valence;
    }vertices;
    struct {
        bcg_scalar_t min_length;
        bcg_scalar_t avg_length;
        bcg_scalar_t max_length;
    }edges;
    struct {
        bcg_scalar_t min_area;
        bcg_scalar_t avg_area;
        bcg_scalar_t max_area;
    }faces;
};

mesh_stats mesh_statistics(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_STATISTICS_H
