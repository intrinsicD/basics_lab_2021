//
// Created by alex on 31.03.21.
//

#include "bcg_graph_dijkstra.h"
#include <queue>
#include <vector>

namespace bcg {

struct front_node {
    front_node(vertex_handle v_to, bcg_scalar_t accumulated_path_length) : v_to(v_to), accumulated_path_length(
            accumulated_path_length) {}

    vertex_handle v_to;
    bcg_scalar_t accumulated_path_length;
};

void graph_shortest_paths_from(halfedge_graph &graph, vertex_handle v_start) {
    graph_shortest_paths_from(graph, v_start, property<bcg_scalar_t, 1>(), property<VectorS<3>, 3>());
}

void graph_shortest_paths_from(halfedge_graph &graph, vertex_handle v_start, property<bcg_scalar_t, 1> heuristic,
                               property<VectorS<3>, 3> guide_vectorfield) {
    auto v_visited = graph.vertices.get_or_add<bcg_scalar_t, 1>("v_visited");
    v_visited.reset(0);
    auto v_dijkstra_distance = graph.vertices.get_or_add<bcg_scalar_t, 1>("v_dijkstra_distance");
    v_dijkstra_distance.reset(std::numeric_limits<bcg_scalar_t>::max());

    auto cmp = [](const front_node &a, const front_node &b) {
        return a.accumulated_path_length > b.accumulated_path_length;
    };

    std::priority_queue<front_node, std::vector<front_node>, decltype(cmp)> queue(cmp);
    queue.emplace(v_start, 0);
    v_visited[v_start] = 1;
    //In debug mode this does not seem to be a valid heap. Rethink Dijkstra!!

    while (!queue.empty()) {
        auto fn = queue.top();
        queue.pop();

        if (fn.accumulated_path_length < v_dijkstra_distance[fn.v_to]) {
            v_dijkstra_distance[fn.v_to] = fn.accumulated_path_length;
            v_visited[fn.v_to] = 2;
        }

        for (const auto &h : graph.get_halfedges(fn.v_to)) {
            auto v_next = graph.get_to_vertex(h);
            if (v_visited[v_next] >= 1) continue;
            auto e = graph.get_edge(h);
            bcg_scalar_t accumulated_path_length = v_dijkstra_distance[fn.v_to] + graph.get_length(e);
            if(heuristic){
                accumulated_path_length += heuristic[e];
            }
            if(guide_vectorfield){
                VectorS<3> dir = graph.positions[v_next] - graph.positions[fn.v_to];
                accumulated_path_length += 1 - std::abs((guide_vectorfield[v_next].normalized().dot(dir.normalized())));
            }
            queue.emplace(v_next, accumulated_path_length);
            v_visited[v_next] = 1;
        }
    }
}

geometric_path graph_shortest_path_between(halfedge_graph &graph, vertex_handle v_start, vertex_handle v_end) {
    return graph_shortest_path_between(graph, v_start, v_end, property<bcg_scalar_t, 1>(), property<VectorS<3>, 3>());
}

geometric_path graph_shortest_path_between(halfedge_graph &graph, vertex_handle v_start, vertex_handle v_end,
                                           property<bcg_scalar_t, 1> heuristic, property<VectorS<3>, 3> guide_vectorfield) {
    auto v_visited = graph.vertices.get_or_add<bcg_scalar_t, 1>("v_visited");
    v_visited.reset(0);
    auto v_dijkstra_distance = graph.vertices.get_or_add<bcg_scalar_t, 1>("v_dijkstra_distance");
    v_dijkstra_distance.reset(std::numeric_limits<bcg_scalar_t>::max());

    auto cmp = [](const front_node &a, const front_node &b) {
        return a.accumulated_path_length > b.accumulated_path_length;
    };

    std::priority_queue<front_node, std::vector<front_node>, decltype(cmp)> queue(cmp);
    queue.emplace(v_start, 0);
    v_visited[v_start] = 1;
    //In debug mode this does not seem to be a valid heap. Rethink Dijkstra!!

    while (!queue.empty()) {
        auto fn = queue.top();
        queue.pop();

        if (fn.accumulated_path_length < v_dijkstra_distance[fn.v_to]) {
            v_dijkstra_distance[fn.v_to] = fn.accumulated_path_length;
            v_visited[fn.v_to] = 2;
        }

        bool finished = false;

        for (const auto &h : graph.get_halfedges(fn.v_to)) {
            auto v_next = graph.get_to_vertex(h);
            if (v_visited[v_next] >= 1) continue;
            auto e = graph.get_edge(h);
            bcg_scalar_t accumulated_path_length = v_dijkstra_distance[fn.v_to] + graph.get_length(e);
            if(heuristic){
                accumulated_path_length += heuristic[e];
            }
            if(guide_vectorfield){
                VectorS<3> dir = graph.positions[v_next] - graph.positions[fn.v_to];
                accumulated_path_length += 1 - std::abs((guide_vectorfield[v_next].normalized().dot(dir.normalized())));
            }
            queue.emplace(v_next, accumulated_path_length);
            v_visited[v_next] = 1;
            if (v_next == v_end) {
                finished = true;
                v_dijkstra_distance[v_end] = accumulated_path_length;
                v_visited[v_end] = 2;
                break;
            }
        }
        if(finished){
            break;
        }
    }

    //backtrack from v_end;
    auto v_shortest_path = graph.vertices.get_or_add<bcg_scalar_t, 1>("v_shortest_path");
    v_shortest_path.reset(0);
    geometric_path result;
    result.length = v_dijkstra_distance[v_end];
    auto v = v_end;
    size_t count = 0;
    halfedge_handle next;
    do {
        if (count > graph.vertices.size()) break;
        bcg_scalar_t smallest_distance = std::numeric_limits<bcg_scalar_t>::max();
        for (const auto &h : graph.get_halfedges(v)) {
            auto vv = graph.get_to_vertex(h);
            if (v_dijkstra_distance[vv] < smallest_distance) {
                smallest_distance = v_dijkstra_distance[vv];
                next = h;
            }
            if(vv == v_start){
                next = h;
            }
        }
        if (!next.is_valid()) {
            break;
        }
        v_shortest_path[v] = 1;
        result.vertices.push_back(v);
        auto e = graph.get_edge(next);
        if(graph.is_valid(e)){
            result.edges.push_back(e);
        }
        v = graph.get_to_vertex(next);
        ++count;
    } while (v != v_start);
    result.vertices.push_back(v_start);
    auto e = graph.get_edge(next);
    if(graph.is_valid(e)){
        result.edges.push_back(e);
    }
    return result;
}

}