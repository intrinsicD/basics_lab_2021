//
// Created by alex on 12.04.21.
//

#ifndef BCG_GRAPHICS_BCG_GRAPH_HARMONIC_SEGMENTATION_TO_PATH_H
#define BCG_GRAPHICS_BCG_GRAPH_HARMONIC_SEGMENTATION_TO_PATH_H

#include "graph/bcg_graph.h"

namespace bcg {

void graph_harmonic_segmentation_to_path(halfedge_graph &graph, property<bcg_scalar_t, 1> harmonic_field,
                                         const std::vector<bcg_scalar_t> &iso_values, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_GRAPH_HARMONIC_SEGMENTATION_TO_PATH_H
