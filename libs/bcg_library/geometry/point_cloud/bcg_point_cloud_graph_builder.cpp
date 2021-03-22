//
// Created by alex on 30.11.20.
//

#include "bcg_point_cloud_graph_builder.h"

namespace bcg{

halfedge_graph point_cloud_build_knn_graph(vertex_container &vertices, const kdtree_property<bcg_scalar_t> &kdtree, int num_closest){
    halfedge_graph graph;
    graph.vertices.link(vertices);
    graph.positions = graph.vertices.get<VectorS<3>, 3>("v_position");
    graph.vertices_deleted = graph.vertices.get<bool, 1>("v_deleted");
    graph.vconn = graph.vertices.get_or_add<halfedge_graph::vertex_connectivity, 1>("v_connectivity");
    graph.vertices.resize(vertices.size());

    halfedge_handle h;
    for(const auto v : graph.vertices){
        auto result = kdtree.query_knn(graph.positions[v], num_closest);
        assert(result.indices.size() == static_cast<size_t>(num_closest));
        for(const auto &idx : result.indices){
            if(v.idx == idx) continue;
            h = graph.add_edge(v, vertex_handle(idx));
        }

        assert( graph.get_valence(v) == static_cast<size_t>(num_closest - 1));
    }
    return graph;
}

halfedge_graph point_cloud_build_radius_graph(vertex_container &vertices, const kdtree_property<bcg_scalar_t> &kdtree,
                                              bcg_scalar_t radius){
    halfedge_graph graph;
    graph.vertices.link(vertices);
    graph.positions = graph.vertices.get<VectorS<3>, 3>("v_position");
    graph.vertices_deleted = graph.vertices.get<bool, 1>("v_deleted");
    graph.vconn = graph.vertices.get_or_add<halfedge_graph::vertex_connectivity, 1>("v_connectivity");
    graph.vertices.resize(vertices.size());

    halfedge_handle h;
    for(const auto v : graph.vertices){
        auto result = kdtree.query_radius(graph.positions[v], radius);
        for(const auto &idx : result.indices){
            if(v.idx == idx) continue;
            h = graph.add_edge(v, vertex_handle(idx));
        }

        assert( graph.get_valence(v) == result.indices.size() - 1);
    }
    return graph;
}

}