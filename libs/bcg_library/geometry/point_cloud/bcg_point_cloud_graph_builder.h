//
// Created by alex on 30.11.20.
//

#ifndef BCG_GRAPHICS_BCG_POINT_CLOUD_GRAPH_BUILDER_H
#define BCG_GRAPHICS_BCG_POINT_CLOUD_GRAPH_BUILDER_H

#include "bcg_point_cloud.h"
#include "graph/bcg_graph.h"
#include "kdtree/bcg_kdtree.h"

namespace bcg {

halfedge_graph point_cloud_build_knn_graph(vertex_container &vertices, const kdtree_property<bcg_scalar_t> &kdtree, int k);

halfedge_graph point_cloud_build_radius_graph(vertex_container &vertices, const kdtree_property<bcg_scalar_t> &kdtree,
                                              bcg_scalar_t radius);

}

#endif //BCG_GRAPHICS_BCG_POINT_CLOUD_GRAPH_BUILDER_H
