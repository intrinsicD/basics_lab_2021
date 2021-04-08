//
// Created by alex on 07.04.21.
//

#include "bcg_graph_edge_from_vertex_function.h"
#include "tbb/tbb.h"

namespace bcg {

property<bcg_scalar_t, 1>
edge_from_vertex_boolean_function_or(halfedge_graph &graph, const std::string &edge_property_name,
                                     property<bcg_scalar_t, 1> vertex_property, bcg_scalar_t value_true,
                                     bcg_scalar_t value_false, size_t parallel_grain_size) {
    auto edge_property = graph.edges.get_or_add<bcg_scalar_t, 1>(edge_property_name);
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) graph.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    auto v0 = graph.get_vertex(e, 0);
                    auto v1 = graph.get_vertex(e, 1);
                    if (vertex_property[v0] > 0 || vertex_property[v1] > 0) {
                        edge_property[e] = value_true;
                    } else {
                        edge_property[e] = value_false;
                    }
                }
            }
    );

    edge_property.set_dirty();
    return edge_property;
}

property<bcg_scalar_t, 1>
edge_from_vertex_boolean_function_and(halfedge_graph &graph, const std::string &edge_property_name,
                                      property<bcg_scalar_t, 1> vertex_property, bcg_scalar_t value_true,
                                      bcg_scalar_t value_false, size_t parallel_grain_size) {
    auto edge_property = graph.edges.get_or_add<bcg_scalar_t, 1>(edge_property_name);
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) graph.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    auto v0 = graph.get_vertex(e, 0);
                    auto v1 = graph.get_vertex(e, 1);
                    if (vertex_property[v0] > 0 && vertex_property[v1] > 0) {
                        edge_property[e] = value_true;
                    } else {
                        edge_property[e] = value_false;
                    }
                }
            }
    );
    edge_property.set_dirty();
    return edge_property;
}

property<bcg_scalar_t, 1>
edge_from_vertex_function_average(halfedge_graph &graph, const std::string &edge_property_name,
                                  property<bcg_scalar_t, 1> vertex_property,
                                  size_t parallel_grain_size){
    auto edge_property = graph.edges.get_or_add<bcg_scalar_t, 1>(edge_property_name);
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) graph.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    auto v0 = graph.get_vertex(e, 0);
                    auto v1 = graph.get_vertex(e, 1);
                    edge_property[e] = (vertex_property[v0] + vertex_property[v1]) * 0.5;
                }
            }
    );
    edge_property.set_dirty();
    return edge_property;
}

property<bcg_scalar_t, 1>
edge_from_vertex_function_min(halfedge_graph &graph, const std::string &edge_property_name,
                              property<bcg_scalar_t, 1> vertex_property, size_t parallel_grain_size){
    auto edge_property = graph.edges.get_or_add<bcg_scalar_t, 1>(edge_property_name);
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) graph.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    auto v0 = graph.get_vertex(e, 0);
                    auto v1 = graph.get_vertex(e, 1);
                    edge_property[e] = std::min(vertex_property[v0], vertex_property[v1]);
                }
            }
    );
    edge_property.set_dirty();
    return edge_property;
}

property<bcg_scalar_t, 1>
edge_from_vertex_function_max(halfedge_graph &graph, const std::string &edge_property_name,
                              property<bcg_scalar_t, 1> vertex_property, size_t parallel_grain_size){
    auto edge_property = graph.edges.get_or_add<bcg_scalar_t, 1>(edge_property_name);
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) graph.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    auto v0 = graph.get_vertex(e, 0);
                    auto v1 = graph.get_vertex(e, 1);
                    edge_property[e] = std::max(vertex_property[v0], vertex_property[v1]);
                }
            }
    );
    edge_property.set_dirty();
    return edge_property;
}

}