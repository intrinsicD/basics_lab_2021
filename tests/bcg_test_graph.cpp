//
// Created by alex on 15.10.20.
//


#include <gtest/gtest.h>

#include "bcg_library/geometry/bcg_graph.h"

using namespace bcg;

TEST(TestSuiteGraph, first){
    halfedge_graph graph;

    EXPECT_TRUE(graph.empty());
    EXPECT_EQ(graph.num_edges(), 0);
    EXPECT_EQ(graph.num_halfedges(), 0);
    EXPECT_FALSE(graph.vertices.is_dirty());
    EXPECT_FALSE(graph.edges.is_dirty());
    EXPECT_FALSE(graph.halfedges.is_dirty());

    auto v0 = graph.add_vertex(VectorS<3>(0, 0, 0));
    auto v1 = graph.add_vertex(VectorS<3>(1, 1, 0));
    auto v2 = graph.add_vertex(VectorS<3>(-2, 2, -1));
    auto v3 = graph.add_vertex(VectorS<3>(0.5, 2, 0));
    auto v4 = graph.add_vertex(VectorS<3>(2, 1.5, 0));
    auto v5 = graph.add_vertex(VectorS<3>(1, 2.5, 1));
    auto v6 = graph.add_vertex(VectorS<3>(1.5, -0.5, 1));

    auto h0 = graph.add_edge(v0, v1);
    auto h1 = graph.get_opposite(h0);
    auto h2 = graph.add_edge(v2, v3);
    auto h3 = graph.get_opposite(h2);
    auto h4 = graph.add_edge(v1, v3);
    auto h5 = graph.get_opposite(h4);
    auto h6 = graph.add_edge(v1, v4);
    auto h7 = graph.get_opposite(h6);
    auto h8 = graph.add_edge(v5, v3);
    auto h9 = graph.get_opposite(h8);
    auto h10 = graph.add_edge(v6, v1);
    auto h11 = graph.get_opposite(h10);
    auto h12 = graph.add_edge(v0, v2);
    auto h13 = graph.get_opposite(h12);

    EXPECT_EQ(h0.idx, size_t(0));
    EXPECT_EQ(h1.idx, size_t(1));
    EXPECT_EQ(h2.idx, size_t(2));
    EXPECT_EQ(h3.idx, size_t(3));
    EXPECT_EQ(h4.idx, size_t(4));
    EXPECT_EQ(h5.idx, size_t(5));
    EXPECT_EQ(h6.idx, size_t(6));
    EXPECT_EQ(h7.idx, size_t(7));
    EXPECT_EQ(h8.idx, size_t(8));
    EXPECT_EQ(h9.idx, size_t(9));
    EXPECT_EQ(h10.idx, size_t(10));
    EXPECT_EQ(h11.idx, size_t(11));
    EXPECT_EQ(h12.idx, size_t(12));
    EXPECT_EQ(h13.idx, size_t(13));

    EXPECT_TRUE(graph.halfedges.is_valid(h0));
    EXPECT_TRUE(graph.halfedges.is_valid(h1));
    EXPECT_TRUE(graph.halfedges.is_valid(h2));
    EXPECT_TRUE(graph.halfedges.is_valid(h3));
    EXPECT_TRUE(graph.halfedges.is_valid(h4));
    EXPECT_TRUE(graph.halfedges.is_valid(h5));
    EXPECT_TRUE(graph.halfedges.is_valid(h6));
    EXPECT_TRUE(graph.halfedges.is_valid(h7));
    EXPECT_TRUE(graph.halfedges.is_valid(h8));
    EXPECT_TRUE(graph.halfedges.is_valid(h9));
    EXPECT_TRUE(graph.halfedges.is_valid(h10));
    EXPECT_TRUE(graph.halfedges.is_valid(h11));
    EXPECT_TRUE(graph.halfedges.is_valid(h12));
    EXPECT_TRUE(graph.halfedges.is_valid(h13));

    EXPECT_EQ(graph.get_from_vertex(h0), v0);
    EXPECT_EQ(graph.get_from_vertex(h1), v1);
    EXPECT_EQ(graph.get_from_vertex(h2), v2);
    EXPECT_EQ(graph.get_from_vertex(h3), v3);
    EXPECT_EQ(graph.get_from_vertex(h4), v1);
    EXPECT_EQ(graph.get_from_vertex(h5), v3);
    EXPECT_EQ(graph.get_from_vertex(h6), v1);
    EXPECT_EQ(graph.get_from_vertex(h7), v4);
    EXPECT_EQ(graph.get_from_vertex(h8), v5);
    EXPECT_EQ(graph.get_from_vertex(h9), v3);
    EXPECT_EQ(graph.get_from_vertex(h10), v6);
    EXPECT_EQ(graph.get_from_vertex(h11), v1);
    EXPECT_EQ(graph.get_from_vertex(h12), v0);
    EXPECT_EQ(graph.get_from_vertex(h13), v2);

    EXPECT_EQ(graph.get_to_vertex(h0), v1);
    EXPECT_EQ(graph.get_to_vertex(h1), v0);
    EXPECT_EQ(graph.get_to_vertex(h2), v3);
    EXPECT_EQ(graph.get_to_vertex(h3), v2);
    EXPECT_EQ(graph.get_to_vertex(h4), v3);
    EXPECT_EQ(graph.get_to_vertex(h5), v1);
    EXPECT_EQ(graph.get_to_vertex(h6), v4);
    EXPECT_EQ(graph.get_to_vertex(h7), v1);
    EXPECT_EQ(graph.get_to_vertex(h8), v3);
    EXPECT_EQ(graph.get_to_vertex(h9), v5);
    EXPECT_EQ(graph.get_to_vertex(h10), v1);
    EXPECT_EQ(graph.get_to_vertex(h11), v6);
    EXPECT_EQ(graph.get_to_vertex(h12), v2);
    EXPECT_EQ(graph.get_to_vertex(h13), v0);

    EXPECT_EQ(graph.get_halfedge(v0), h12);
    EXPECT_EQ(graph.get_halfedge(v1), h11);
    EXPECT_EQ(graph.get_halfedge(v2), h13);
    EXPECT_EQ(graph.get_halfedge(v3), h9);
    EXPECT_EQ(graph.get_halfedge(v4), h7);
    EXPECT_EQ(graph.get_halfedge(v5), h8);
    EXPECT_EQ(graph.get_halfedge(v6), h10);

    EXPECT_EQ(graph.get_next(h0), h11);
    EXPECT_EQ(graph.get_next(h1), h12);
    EXPECT_EQ(graph.get_next(h2), h9);
    EXPECT_EQ(graph.get_next(h3), h13);
    EXPECT_EQ(graph.get_next(h4), h3);
    EXPECT_EQ(graph.get_next(h5), h1);
    EXPECT_EQ(graph.get_next(h6), halfedge_handle());
    EXPECT_EQ(graph.get_next(h7), h4);
    EXPECT_EQ(graph.get_next(h8), h5);
    EXPECT_EQ(graph.get_next(h9), halfedge_handle());
    EXPECT_EQ(graph.get_next(h10), h6);
    EXPECT_EQ(graph.get_next(h11), halfedge_handle());
    EXPECT_EQ(graph.get_next(h12), h2);
    EXPECT_EQ(graph.get_next(h13), h0);

    EXPECT_EQ(graph.get_prev(h0), h13);
    EXPECT_EQ(graph.get_prev(h1), h5);
    EXPECT_EQ(graph.get_prev(h2), h12);
    EXPECT_EQ(graph.get_prev(h3), h4);
    EXPECT_EQ(graph.get_prev(h4), h7);
    EXPECT_EQ(graph.get_prev(h5), h8);
    EXPECT_EQ(graph.get_prev(h6), h10);
    EXPECT_EQ(graph.get_prev(h7), halfedge_handle());
    EXPECT_EQ(graph.get_prev(h8), halfedge_handle());
    EXPECT_EQ(graph.get_prev(h9), h2);
    EXPECT_EQ(graph.get_prev(h10), halfedge_handle());
    EXPECT_EQ(graph.get_prev(h11), h0);
    EXPECT_EQ(graph.get_prev(h12), h1);
    EXPECT_EQ(graph.get_prev(h13), h3);

    size_t count = 0;
    for(const auto e : graph.edges){
        EXPECT_TRUE(graph.edges.is_valid(e));
        ++count;
    }
    EXPECT_EQ(count, 7);

    auto h_visited = graph.halfedges.add<size_t, 1>("h_visited", 0);
    for(const auto v : graph.vertices){
        for(const auto he : graph.get_halfedges(v)){
            ++h_visited[he];
        }
    }

    count = 0;
    for(const auto he : graph.halfedges){
        EXPECT_TRUE(graph.halfedges.is_valid(he));
        EXPECT_EQ(h_visited[he], 1);
        if(h_visited[he] > 0){
            ++count;
        }
    }

    EXPECT_EQ(count, graph.num_halfedges());

    count = 0;
    auto v_visited = graph.vertices.add<size_t, 1>("v_visited", 0);
    for(const auto vv : graph.get_vertices(v1)){
        EXPECT_TRUE(graph.vertices.is_valid(vv));
        v_visited[vv] = 1;
        ++count;
    }

    EXPECT_TRUE(v_visited[v0]);
    EXPECT_TRUE(v_visited[v3]);
    EXPECT_TRUE(v_visited[v4]);
    EXPECT_TRUE(v_visited[v6]);
    EXPECT_FALSE(v_visited[v2]);
    EXPECT_FALSE(v_visited[v5]);
    EXPECT_FALSE(v_visited[v1]);

    EXPECT_EQ(count, 4);

    EXPECT_EQ(graph.get_valence(v0), 2);
    EXPECT_EQ(graph.get_valence(v1), 4);
    EXPECT_EQ(graph.get_valence(v2), 2);
    EXPECT_EQ(graph.get_valence(v3), 3);
    EXPECT_EQ(graph.get_valence(v4), 1);
    EXPECT_EQ(graph.get_valence(v5), 1);
    EXPECT_EQ(graph.get_valence(v6), 1);

    auto result = graph.find_closest_edge(graph.get_center(graph.get_edge(h0)));
    EXPECT_EQ(result, v0);

    graph.remove_edge(graph.get_edge(h0));

    EXPECT_EQ(graph.get_halfedge(v0), h12);
    EXPECT_EQ(graph.get_halfedge(v1), h11);
    EXPECT_TRUE(graph.halfedges.is_dirty());
    EXPECT_TRUE(graph.edges.is_dirty());
    EXPECT_TRUE(graph.has_garbage());

    graph.garbage_collection();

    EXPECT_TRUE(graph.halfedges.is_dirty());
    EXPECT_TRUE(graph.edges.is_dirty());
    EXPECT_FALSE(graph.has_garbage());

    auto v_isolated = graph.add_vertex(VectorS<3>(10,11,1));
    EXPECT_TRUE(graph.is_isolated(v_isolated));
}