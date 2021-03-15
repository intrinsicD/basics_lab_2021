//
// Created by alex on 15.10.20.
//


#include <gtest/gtest.h>

#include "geometry/graph/bcg_graph.h"
#include "geometry/aligned_box/bcg_aligned_box.h"
#include "math/vector/bcg_vector_map_eigen.h"
#include "math/matrix/bcg_matrix_map_eigen.h"

using namespace bcg;

TEST(TestSuiteGraph, first){
    //TODO Fix me, halfedge graph is not correct for aligned_box3!
    halfedge_graph graph;

    EXPECT_TRUE(graph.empty());
    EXPECT_EQ(graph.num_edges(), 0);
    EXPECT_EQ(graph.num_halfedges(), 0);
    EXPECT_FALSE(graph.vertices.is_dirty());
    EXPECT_FALSE(graph.edges.is_dirty());
    EXPECT_FALSE(graph.halfedges.is_dirty());

    aligned_box3 aabb = aligned_box3::unit();

    auto vertices = get_vetices(aabb);
    auto edges = get_edges(aabb);
    std::vector<size_t> V;
    for(const VectorS<3> &v:vertices){
        V.push_back(graph.add_vertex(v));
    }

    std::vector<size_t> H;
    for(const auto e:edges){
        H.push_back(graph.add_edge(V[e[0]], V[e[1]]));
        H.push_back(graph.get_opposite(H.back()));
    }

    std::cout << "v: " << MapConst(V).transpose() << "\n";
    std::cout << "h: " << MapConst(H).transpose() << "\n";
    std::cout << "e:\n" << MapConst(edges).transpose() << "\n";

    for(size_t i = 0; i < H.size(); ++i){
        EXPECT_EQ(H[i], i);
        EXPECT_TRUE(graph.halfedges.is_valid(H[i]));
    }

    EXPECT_EQ(graph.get_from_vertex(H[0]).idx, V[0]);
    EXPECT_EQ(graph.get_from_vertex(H[1]).idx, V[1]);
    EXPECT_EQ(graph.get_from_vertex(H[2]).idx, V[0]);
    EXPECT_EQ(graph.get_from_vertex(H[3]).idx, V[2]);
    EXPECT_EQ(graph.get_from_vertex(H[4]).idx, V[0]);
    EXPECT_EQ(graph.get_from_vertex(H[5]).idx, V[4]);
    EXPECT_EQ(graph.get_from_vertex(H[6]).idx, V[1]);
    EXPECT_EQ(graph.get_from_vertex(H[7]).idx, V[3]);
    EXPECT_EQ(graph.get_from_vertex(H[8]).idx, V[1]);
    EXPECT_EQ(graph.get_from_vertex(H[9]).idx, V[5]);
    EXPECT_EQ(graph.get_from_vertex(H[10]).idx, V[2]);
    EXPECT_EQ(graph.get_from_vertex(H[11]).idx, V[3]);
    EXPECT_EQ(graph.get_from_vertex(H[12]).idx, V[2]);
    EXPECT_EQ(graph.get_from_vertex(H[13]).idx, V[6]);
    EXPECT_EQ(graph.get_from_vertex(H[14]).idx, V[3]);
    EXPECT_EQ(graph.get_from_vertex(H[15]).idx, V[7]);
    EXPECT_EQ(graph.get_from_vertex(H[16]).idx, V[4]);
    EXPECT_EQ(graph.get_from_vertex(H[17]).idx, V[5]);
    EXPECT_EQ(graph.get_from_vertex(H[18]).idx, V[4]);
    EXPECT_EQ(graph.get_from_vertex(H[19]).idx, V[6]);
    EXPECT_EQ(graph.get_from_vertex(H[20]).idx, V[5]);
    EXPECT_EQ(graph.get_from_vertex(H[21]).idx, V[7]);
    EXPECT_EQ(graph.get_from_vertex(H[22]).idx, V[6]);

    EXPECT_EQ(graph.get_to_vertex(H[0]).idx, V[1]);
    EXPECT_EQ(graph.get_to_vertex(H[1]).idx, V[0]);
    EXPECT_EQ(graph.get_to_vertex(H[2]).idx, V[2]);
    EXPECT_EQ(graph.get_to_vertex(H[3]).idx, V[0]);
    EXPECT_EQ(graph.get_to_vertex(H[4]).idx, V[4]);
    EXPECT_EQ(graph.get_to_vertex(H[5]).idx, V[0]);
    EXPECT_EQ(graph.get_to_vertex(H[6]).idx, V[3]);
    EXPECT_EQ(graph.get_to_vertex(H[7]).idx, V[1]);
    EXPECT_EQ(graph.get_to_vertex(H[8]).idx, V[5]);
    EXPECT_EQ(graph.get_to_vertex(H[9]).idx, V[1]);
    EXPECT_EQ(graph.get_to_vertex(H[10]).idx, V[3]);
    EXPECT_EQ(graph.get_to_vertex(H[11]).idx, V[2]);
    EXPECT_EQ(graph.get_to_vertex(H[12]).idx, V[6]);
    EXPECT_EQ(graph.get_to_vertex(H[13]).idx, V[2]);
    EXPECT_EQ(graph.get_to_vertex(H[14]).idx, V[7]);
    EXPECT_EQ(graph.get_to_vertex(H[15]).idx, V[3]);
    EXPECT_EQ(graph.get_to_vertex(H[16]).idx, V[5]);
    EXPECT_EQ(graph.get_to_vertex(H[17]).idx, V[4]);
    EXPECT_EQ(graph.get_to_vertex(H[18]).idx, V[6]);
    EXPECT_EQ(graph.get_to_vertex(H[19]).idx, V[4]);
    EXPECT_EQ(graph.get_to_vertex(H[20]).idx, V[7]);
    EXPECT_EQ(graph.get_to_vertex(H[21]).idx, V[5]);
    EXPECT_EQ(graph.get_to_vertex(H[22]).idx, V[7]);

    EXPECT_EQ(graph.get_halfedge(V[0]).idx, H[4]);
    EXPECT_EQ(graph.get_halfedge(V[1]).idx, H[8]);
    EXPECT_EQ(graph.get_halfedge(V[2]).idx, H[12]);
    EXPECT_EQ(graph.get_halfedge(V[3]).idx, H[14]);
    EXPECT_EQ(graph.get_halfedge(V[4]).idx, H[18]);
    EXPECT_EQ(graph.get_halfedge(V[5]).idx, H[20]);
    EXPECT_EQ(graph.get_halfedge(V[6]).idx, H[22]);
    EXPECT_EQ(graph.get_halfedge(V[7]).idx, H[2]);

    EXPECT_EQ(graph.get_next(H[0]).idx, V[1]);
    EXPECT_EQ(graph.get_next(H[1]).idx, V[0]);
    EXPECT_EQ(graph.get_next(H[2]).idx, V[2]);
    EXPECT_EQ(graph.get_next(H[3]).idx, V[0]);
    EXPECT_EQ(graph.get_next(H[4]).idx, V[3]);
    EXPECT_EQ(graph.get_next(H[5]).idx, V[0]);
    EXPECT_EQ(graph.get_next(H[6]).idx, V[3]);
    EXPECT_EQ(graph.get_next(H[7]).idx, V[1]);
    EXPECT_EQ(graph.get_next(H[8]).idx, V[5]);
    EXPECT_EQ(graph.get_next(H[9]).idx, V[1]);
    EXPECT_EQ(graph.get_next(H[10]).idx, V[3]);
    EXPECT_EQ(graph.get_next(H[11]).idx, V[2]);
    EXPECT_EQ(graph.get_next(H[12]).idx, V[6]);
    EXPECT_EQ(graph.get_next(H[13]).idx, V[2]);
    EXPECT_EQ(graph.get_next(H[13]).idx, V[7]);
    EXPECT_EQ(graph.get_next(H[14]).idx, V[3]);
    EXPECT_EQ(graph.get_next(H[15]).idx, V[5]);
    EXPECT_EQ(graph.get_next(H[16]).idx, V[4]);
    EXPECT_EQ(graph.get_next(H[17]).idx, V[6]);
    EXPECT_EQ(graph.get_next(H[18]).idx, V[4]);
    EXPECT_EQ(graph.get_next(H[19]).idx, V[7]);
    EXPECT_EQ(graph.get_next(H[20]).idx, V[5]);
    EXPECT_EQ(graph.get_next(H[21]).idx, V[7]);
    EXPECT_EQ(graph.get_next(H[22]).idx, V[6]);

    EXPECT_EQ(graph.get_prev(H[0]).idx, V[1]);
    EXPECT_EQ(graph.get_prev(H[1]).idx, V[0]);
    EXPECT_EQ(graph.get_prev(H[2]).idx, V[2]);
    EXPECT_EQ(graph.get_prev(H[3]).idx, V[0]);
    EXPECT_EQ(graph.get_prev(H[4]).idx, V[3]);
    EXPECT_EQ(graph.get_prev(H[5]).idx, V[0]);
    EXPECT_EQ(graph.get_prev(H[6]).idx, V[3]);
    EXPECT_EQ(graph.get_prev(H[7]).idx, V[1]);
    EXPECT_EQ(graph.get_prev(H[8]).idx, V[5]);
    EXPECT_EQ(graph.get_prev(H[9]).idx, V[1]);
    EXPECT_EQ(graph.get_prev(H[10]).idx, V[3]);
    EXPECT_EQ(graph.get_prev(H[11]).idx, V[2]);
    EXPECT_EQ(graph.get_prev(H[12]).idx, V[6]);
    EXPECT_EQ(graph.get_prev(H[13]).idx, V[2]);
    EXPECT_EQ(graph.get_prev(H[13]).idx, V[7]);
    EXPECT_EQ(graph.get_prev(H[14]).idx, V[3]);
    EXPECT_EQ(graph.get_prev(H[15]).idx, V[5]);
    EXPECT_EQ(graph.get_prev(H[16]).idx, V[4]);
    EXPECT_EQ(graph.get_prev(H[17]).idx, V[6]);
    EXPECT_EQ(graph.get_prev(H[18]).idx, V[4]);
    EXPECT_EQ(graph.get_prev(H[19]).idx, V[7]);
    EXPECT_EQ(graph.get_prev(H[20]).idx, V[5]);
    EXPECT_EQ(graph.get_prev(H[21]).idx, V[7]);
    EXPECT_EQ(graph.get_prev(H[22]).idx, V[6]);

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
    for(const auto vv : graph.get_vertices(V[1])){
        EXPECT_TRUE(graph.vertices.is_valid(vv));
        v_visited[vv] = 1;
        ++count;
    }

    EXPECT_TRUE(v_visited[V[0]]);
    EXPECT_TRUE(v_visited[V[1]]);
    EXPECT_TRUE(v_visited[V[2]]);
    EXPECT_TRUE(v_visited[V[4]]);


    EXPECT_EQ(count, 4);

    EXPECT_EQ(graph.get_valence(V[0]), 3);
    EXPECT_EQ(graph.get_valence(V[1]), 3);
    EXPECT_EQ(graph.get_valence(V[2]), 3);
    EXPECT_EQ(graph.get_valence(V[3]), 3);
    EXPECT_EQ(graph.get_valence(V[4]), 3);
    EXPECT_EQ(graph.get_valence(V[5]), 3);
    EXPECT_EQ(graph.get_valence(V[6]), 3);

    auto result = graph.find_closest_edge(graph.get_center(graph.get_edge(H[0])));
    EXPECT_EQ(result, graph.get_edge(H[0]));

    graph.remove_edge(graph.get_edge(H[0]));

    EXPECT_EQ(graph.get_halfedge(V[0]).idx, 4);
    EXPECT_EQ(graph.get_halfedge(V[1]).idx, 8);
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