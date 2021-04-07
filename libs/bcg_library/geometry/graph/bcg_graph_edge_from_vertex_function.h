//
// Created by alex on 07.04.21.
//

#ifndef BCG_GRAPHICS_BCG_GRAPH_EDGE_FROM_VERTEX_FUNCTION_H
#define BCG_GRAPHICS_BCG_GRAPH_EDGE_FROM_VERTEX_FUNCTION_H

#include "bcg_graph.h"

namespace bcg {

property<bcg_scalar_t, 1>
edge_from_vertex_boolean_function_or(halfedge_graph &graph, const std::string &edge_property_name,
                                     property<bcg_scalar_t, 1> vertex_property, bcg_scalar_t value_true, bcg_scalar_t value_false,
                                     size_t parallel_grain_size = 1024);

property<bcg_scalar_t, 1>
edge_from_vertex_boolean_function_and(halfedge_graph &graph, const std::string &edge_property_name,
                                      property<bcg_scalar_t, 1> vertex_property, bcg_scalar_t value, bcg_scalar_t value_false,
                                      size_t parallel_grain_size = 1024);

}


#endif //BCG_GRAPHICS_BCG_GRAPH_EDGE_FROM_VERTEX_FUNCTION_H
