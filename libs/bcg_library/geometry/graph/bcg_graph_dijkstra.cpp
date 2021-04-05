//
// Created by alex on 31.03.21.
//

#include "bcg_graph_dijkstra.h"
#include <queue>
#include <vector>

namespace bcg {

struct Node {
    Node(vertex_handle v, property<bcg_scalar_t, 1> *distance) : v(v), distance(distance) {}

    vertex_handle v;
    property<bcg_scalar_t, 1> *distance;
};

struct Greater {
    bool operator()(const Node &a, const Node &b) const {
        return (*a.distance)[a.v] > (*b.distance)[b.v];
    }
};

void graph_shortest_paths_from(halfedge_graph &graph, vertex_handle v_start) {
    graph_shortest_paths_from(graph, v_start, property<bcg_scalar_t, 1>(), property<VectorS<3>, 3>());
}

void graph_shortest_paths_from(halfedge_graph &graph, vertex_handle v_start, property<bcg_scalar_t, 1> heuristic,
                               property<VectorS<3>, 3> guide_vectorfield) {
    auto v_dijkstra_distance = graph.vertices.get_or_add<bcg_scalar_t, 1>("v_dijkstra_distance");
    v_dijkstra_distance.reset(std::numeric_limits<bcg_scalar_t>::max());
    auto v_visited = graph.vertices.get_or_add<bool, 1>("v_visited", false);
    v_visited.reset(false);

    auto greater = [](const Node &a, const Node &b) {
        return (*a.distance)[a.v] > (*b.distance)[b.v];
    };

    std::priority_queue<Node, std::vector<Node>, decltype(greater)> unvisited_queue(greater);
    unvisited_queue.emplace(v_start, &v_dijkstra_distance);

    v_dijkstra_distance[v_start] = 0;
    while (!unvisited_queue.empty()) {
        auto current = unvisited_queue.top();
        unvisited_queue.pop();

        const VectorS<3> &p_current = graph.positions[current.v];
        for (const auto &h: graph.get_halfedges(current.v)) {
            auto v = graph.get_to_vertex(h);
            if (v_visited[v]) continue;

            VectorS<3> dir = graph.positions[v] - p_current;
            bcg_scalar_t delta = dir.norm();

            if (heuristic) {
                delta *= (std::abs(heuristic[v]) + scalar_eps);
            }

            if (guide_vectorfield) {
                delta += 1 - std::abs((guide_vectorfield[v].normalized().dot(dir.normalized())));
            }

            bcg_scalar_t length = v_dijkstra_distance[current.v] + delta;
            if (length < v_dijkstra_distance[v]) {
                v_dijkstra_distance[v] = length;
                unvisited_queue.emplace(v, &v_dijkstra_distance);
            }
        }
        v_visited[current.v] = true;
    }
    graph.vertices.remove(v_visited);
}

Path graph_shortest_path_between(halfedge_graph &graph, vertex_handle v_start, vertex_handle v_end) {
    return graph_shortest_path_between(graph, v_start, v_end, property<bcg_scalar_t, 1>(), property<VectorS<3>, 3>());
}

struct path_edge {
    halfedge_graph *graph;
    vertex_handle v_start, v_end;
    property<bcg_scalar_t, 1> e_dijkstra_distance;
    edge_handle e;
};

Path graph_shortest_path_between(halfedge_graph &graph, vertex_handle v_start, vertex_handle v_end,
                                 property<bcg_scalar_t, 1> heuristic, property<VectorS<3>, 3> guide_vectorfield) {
    auto v_visited = graph.vertices.get_or_add<bcg_scalar_t, 1>("v_visited");
    v_visited.reset(0);
    auto v_dijkstra_distance = graph.vertices.get_or_add<bcg_scalar_t, 1>("v_dijkstra_distance");
    v_dijkstra_distance.reset(std::numeric_limits<bcg_scalar_t>::max());

    struct front_node {
        front_node(vertex_handle v_to, bcg_scalar_t accumulated_path_length) : v_to(v_to), accumulated_path_length(
                accumulated_path_length) {}

        vertex_handle v_to;
        bcg_scalar_t accumulated_path_length;
    };

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
            bcg_scalar_t accumulated_path_length = v_dijkstra_distance[fn.v_to] + graph.get_length(graph.get_edge(h));
            if(heuristic){
                accumulated_path_length += heuristic[v_next];
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
    Path result;
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
        result.edges.push_back(graph.get_edge(next));
        v = graph.get_to_vertex(next);
        ++count;
    } while (v != v_start);
    result.vertices.push_back(v_start);
    result.edges.push_back(graph.get_edge(next));
    return result;
}


Path graph_shortest_path_between1(halfedge_graph &graph, vertex_handle v_start, vertex_handle v_end,
                                  property<bcg_scalar_t, 1> heuristic, property<VectorS<3>, 3> guide_vectorfield) {
    auto v_dijkstra_distance = graph.vertices.get_or_add<bcg_scalar_t, 1>("v_dijkstra_distance");
    v_dijkstra_distance.reset(std::numeric_limits<bcg_scalar_t>::max());
    auto v_visited = graph.vertices.get_or_add<bool, 1>("v_visited", false);
    v_visited.reset(false);

    auto greater = [](const Node &a, const Node &b) {
        return (*a.distance)[a.v] > (*b.distance)[b.v];
    };
    //In debug mode this does not seem to be a valid heap. Rethink Dijkstra!!
    std::priority_queue<Node, std::vector<Node>, decltype(greater)> unvisited_queue(greater);
    unvisited_queue.emplace(v_start, &v_dijkstra_distance);

    v_dijkstra_distance[v_start] = 0;
    bool finished = false;
    while (!unvisited_queue.empty()) {
        auto current = unvisited_queue.top();
        unvisited_queue.pop();

        const VectorS<3> &p_current = graph.positions[current.v];
        for (const auto &h: graph.get_halfedges(current.v)) {
            auto v = graph.get_to_vertex(h);
            if (v_visited[v]) continue;

            VectorS<3> dir = graph.positions[v] - p_current;
            bcg_scalar_t delta = dir.norm();

            if (heuristic) {
                //delta += (1 - std::exp(- std::abs(heuristic[v]) * std::abs(heuristic[v]) / (2.0 * 9)));
                delta += std::abs(heuristic[v]);
            }

            if (guide_vectorfield) {
                delta += 1 - std::abs((guide_vectorfield[v].normalized().dot(dir.normalized())));
            }

            bcg_scalar_t length = v_dijkstra_distance[current.v] + delta;
            if (length < v_dijkstra_distance[v]) {
                v_dijkstra_distance[v] = length;
                unvisited_queue.emplace(v, &v_dijkstra_distance);
            }
            finished = v == v_end;
            if (finished) break;
        }
        v_visited[current.v] = true;
        if (finished) break;
    }

    auto v_shortest_path = graph.vertices.get_or_add<bcg_scalar_t, 1>("v_shortest_path");
    v_shortest_path.reset(0);
    Path result;
    result.length = v_dijkstra_distance[v_end];
    auto v = v_end;
    size_t count = 0;
    do {
        if (count > graph.vertices.size()) break;
        bcg_scalar_t smallest_distance = std::numeric_limits<bcg_scalar_t>::max();
        halfedge_handle next;
        for (const auto &h : graph.get_halfedges(v)) {
            auto vv = graph.get_to_vertex(h);
            if (v_dijkstra_distance[vv] < smallest_distance) {
                smallest_distance = v_dijkstra_distance[vv];
                next = h;
            }
        }
        if (!next.is_valid()) break;
        v_shortest_path[v] = 1;
        result.vertices.push_back(v);
        result.edges.push_back(graph.get_edge(next));
        v = graph.get_to_vertex(next);
        ++count;
    } while (v != v_start);

    graph.vertices.remove(v_visited);
    return result;
}


}