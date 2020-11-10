//
// Created by alex on 10.10.20.
//

#include <cassert>
#include "bcg_graph.h"
#include "utils/bcg_stl_utils.h"
#include "distance_query/bcg_distance_segment_point.h"

namespace bcg {

halfedge_graph::halfedge_graph() : point_cloud(),
                                   vconn(vertices.add<vertex_connectivity, 1>("connectivity")),
                                   hconn(halfedges.add<halfedge_connectivity, 4>("connectivity")),
                                   halfedges_deleted(halfedges.add<bool, 1>("deleted")),
                                   edges_deleted(edges.add<bool, 1>("deleted")),
                                   size_halfedges_deleted(0),
                                   size_edges_deleted(0) {

}

void halfedge_graph::assign(const halfedge_graph &other) {
    if (this != &other) {
        point_cloud::assign(other);
        halfedges.remove_all();
        edges.remove_all();

        vconn = vertices.get_or_add<vertex_connectivity, 1>("connectivity");
        hconn = halfedges.get_or_add<halfedge_connectivity, 4>("connectivity");

        halfedges_deleted = halfedges.get_or_add<bool, 1>("deleted");
        edges_deleted = edges.get_or_add<bool, 1>("deleted");

        vconn.vector() = other.vconn.vector();
        hconn.vector() = other.hconn.vector();

        halfedges_deleted.vector() = other.halfedges_deleted.vector();
        edges_deleted.vector() = other.edges_deleted.vector();

        edges.resize(other.edges.size());
        halfedges.resize(other.halfedges.size());

        size_halfedges_deleted = other.size_halfedges_deleted;
        size_edges_deleted = other.size_edges_deleted;
    }
}

halfedge_graph &halfedge_graph::operator=(const halfedge_graph &other) {
    if (this != &other) {
        vertices = other.vertices;
        halfedges = other.halfedges;
        edges = other.edges;
        positions = vertices.get_or_add<position_t, 3>("position");

        vconn = vertices.get_or_add<vertex_connectivity, 1>("connectivity");
        hconn = halfedges.get_or_add<halfedge_connectivity, 4>("connectivity");

        vertices_deleted = vertices.get_or_add<bool, 1>("deleted");
        halfedges_deleted = halfedges.get_or_add<bool, 1>("deleted");
        edges_deleted = edges.get_or_add<bool, 1>("deleted");

        size_vertices_deleted = other.size_vertices_deleted;
        size_halfedges_deleted = other.size_halfedges_deleted;
        size_edges_deleted = other.size_edges_deleted;
    }
    return *this;
}

size_t halfedge_graph::num_edges() const {
    return edges.size() - size_edges_deleted;
}

size_t halfedge_graph::num_halfedges() const {
    return halfedges.size() - size_halfedges_deleted;
}

bool halfedge_graph::has_garbage() const {
    return size_edges_deleted > 0 || size_edges_deleted > 0 || point_cloud::has_garbage();
}

void halfedge_graph::garbage_collection() {
    if (!has_garbage()) return;

    size_t nV = vertices.size();
    size_t nE = edges.size();
    size_t nH = halfedges.size();

    auto vmap = vertices.add<vertex_handle, 1>("garbage_collection");
    auto hmap = halfedges.add<halfedge_handle, 1>("garbage_collection");

    for (size_t i = 0; i < nV; ++i) {
        vmap[i] = vertex_handle(i);
    }

    for (size_t i = 0; i < nH; ++i) {
        hmap[i] = halfedge_handle(i);
    }

    if (nV > 0) {
        size_t i0 = 0;
        size_t i1 = nV - 1;

        while (true) {
            // find first deleted and last un-deleted
            while (!vertices_deleted[i0] && i0 < i1) {
                ++i0;
            }
            while (vertices_deleted[i1] && i0 < i1) {
                --i1;
            }
            if (i0 >= i1) break;

            // swap
            vertices.swap(i0, i1);
        }

        // remember new size
        nV = vertices_deleted[i0] ? i0 : i0 + 1;
    }

    if (nE > 0) {
        size_t i0 = 0;
        size_t i1 = nE - 1;

        while (true) {
            // find first deleted and last un-deleted
            while (!edges_deleted[i0] && i0 < i1) {
                ++i0;
            }
            while (edges_deleted[i1] && i0 < i1) {
                --i1;
            }
            if (i0 >= i1) break;

            // swap
            edges.swap(i0, i1);
            halfedges.swap(2 * i0, 2 * i1);
            halfedges.swap(2 * i0 + 1, 2 * i1 + 1);
        }

        // remember new size
        nE = edges_deleted[i0] ? i0 : i0 + 1;
        nH = 2 * nE;
    }

    // update vertex connectivity
    for (size_t v = 0; v < nV; ++v) {
        if (!is_isolated(v)) {
            set_halfedge(v, hmap[get_halfedge(v)]);
        }
    }

    for (size_t h = 0; h < nH; ++h) {
        set_vertex(h, vmap[get_to_vertex(h).idx]);
        auto next = get_next(h);
        if (halfedges.is_valid(next)) {
            set_next(h, hmap[next]);
        }
    }

    vertices.remove(vmap);
    halfedges.remove(hmap);

    vertices.resize(nV);
    vertices.free_unused_memory();
    halfedges.resize(nH);
    halfedges.free_unused_memory();
    edges.resize(nE);
    edges.free_unused_memory();

    size_vertices_deleted = 0;
    size_halfedges_deleted = 0;
    size_edges_deleted = 0;
    assert(!has_garbage());
    assert(vertices.is_dirty());
    assert(edges.is_dirty());
    assert(halfedges.is_dirty());
}

bool halfedge_graph::is_isolated(vertex_handle v) const {
    return !halfedges.is_valid(get_halfedge(v));
}

bool halfedge_graph::is_boundary(vertex_handle v) const {
    size_t count(0);
    for (const auto vj : get_vertices(v)) {
        if (vj.is_valid()) {
            ++count;
        }
        if (count > 1) return false;
    }
    return true;
}

bool halfedge_graph::is_boundary(halfedge_handle h) const {
    return get_next(h) == get_opposite(h);
}

bool halfedge_graph::is_boundary(edge_handle e) const {
    return is_boundary(get_halfedge(e, 0)) || is_boundary(get_halfedge(e, 1));
}

void halfedge_graph::delete_vertex(vertex_handle v) {
    if (vertices_deleted[v]) return;

    for (const auto h : get_halfedges(v)) {
        remove_edge(get_edge(h));
    }

    point_cloud::delete_vertex(v);
}

halfedge_handle halfedge_graph::get_halfedge(vertex_handle v) const {
    return vconn[v].h;
}

void halfedge_graph::set_halfedge(vertex_handle v, halfedge_handle h) {
    vconn[v].h = h;
}

vertex_handle halfedge_graph::get_to_vertex(halfedge_handle h) const {
    return hconn[h].v;
}

void halfedge_graph::set_vertex(halfedge_handle h, vertex_handle v) {
    hconn[h].v = v;
}

vertex_handle halfedge_graph::get_from_vertex(halfedge_handle h) const {
    return get_to_vertex(get_opposite(h));
}

halfedge_handle halfedge_graph::get_next(halfedge_handle h) const {
    return hconn[h].nh;
}

void halfedge_graph::set_next(halfedge_handle h, halfedge_handle nh) {
    hconn[h].nh = nh;
    hconn[nh].ph = h;
}

halfedge_handle halfedge_graph::get_prev(halfedge_handle h) const {
    return hconn[h].ph;
}

halfedge_handle halfedge_graph::get_opposite(halfedge_handle h) const {
    return halfedge_handle((h.idx & 1) ? h.idx - 1 : h.idx + 1);
}

halfedge_handle halfedge_graph::rotate_cw(halfedge_handle h) const {
    return get_next(get_opposite(h));
}

halfedge_handle halfedge_graph::rotate_ccw(halfedge_handle h) const {
    return get_opposite(get_prev(h));
}

edge_handle halfedge_graph::get_edge(halfedge_handle h) const {
    return edge_handle((h.idx >> 1));
}

halfedge_handle halfedge_graph::get_halfedge(edge_handle e, bool i) const {
    return halfedge_handle((e.idx << 1) + i);
}

vertex_handle halfedge_graph::get_vertex(edge_handle e, bool i) const {
    return get_to_vertex(get_halfedge(e, i));
}

size_t halfedge_graph::get_valence(vertex_handle v) const {
    size_t count(0);
    for (const auto vj : get_vertices(v)) {
        if (vj.is_valid()) {
            ++count;
        }
    }
    return count;
}

bcg_scalar_t halfedge_graph::get_length(edge_handle e) const {
    return get_vector(e).norm();
}

point_cloud::position_t halfedge_graph::get_center(halfedge_handle h) const {
    return (positions[get_to_vertex(h)] + positions[get_from_vertex(h)]) / 2.0f;
}

point_cloud::position_t halfedge_graph::get_center(edge_handle e) const {
    return get_center(get_halfedge(e, 0));
}

point_cloud::position_t halfedge_graph::get_vector(halfedge_handle h) const {
    return positions[get_to_vertex(h)] - positions[get_from_vertex(h)];
}

point_cloud::position_t halfedge_graph::get_vector(edge_handle e) const {
    return get_vector(get_halfedge(e, 0));
}

halfedge_handle halfedge_graph::add_edge(vertex_handle v0, vertex_handle v1) {
    auto h = find_halfedge(v0, v1);
    if (!h.is_valid()) {
        h = new_edge(v0, v1);
    }

    auto o = get_opposite(h);

    auto v0_outgoing = get_halfedge(v0);

    if (halfedges.is_valid(v0_outgoing)) {
        auto prev_v0_outgoing = get_prev(v0_outgoing);
        if (halfedges.is_valid(prev_v0_outgoing)) {
            set_next(prev_v0_outgoing, h);
            set_next(o, v0_outgoing);
        } else {
            set_next(get_opposite(v0_outgoing), h);
            set_next(o, v0_outgoing);
        }
    }

    set_halfedge(v0, h);

    auto v1_outgoing = get_halfedge(v1);
    if (halfedges.is_valid(v1_outgoing)) {
        auto prev_v1_outgoing = get_prev(v1_outgoing);
        if (halfedges.is_valid(prev_v1_outgoing)) {
            set_next(h, v1_outgoing);
            set_next(prev_v1_outgoing, o);
        } else {
            set_next(h, v1_outgoing);
            set_next(get_opposite(v1_outgoing), o);
        }
    }

    set_halfedge(v1, o);

    assert(halfedges.is_dirty());
    assert(edges.is_dirty());
    return h;
}

void halfedge_graph::remove_edge(edge_handle e) {
    if (edges_deleted[e]) return;

    auto h0 = get_halfedge(e, 0);
    auto h1 = get_halfedge(e, 1);

    auto from_v = get_to_vertex(h1);
    auto to_v = get_to_vertex(h0);

    if (halfedges.is_valid(h0)) {
        auto p = get_prev(h0);
        auto n = get_next(h1);
        if (find_halfedge(get_to_vertex(n), from_v).is_valid()) {
            set_next(p, n);
        }
    }
    if (halfedges.is_valid(h1)) {
        auto p = get_prev(h1);
        auto n = get_next(h0);
        if (find_halfedge(get_to_vertex(n), to_v).is_valid()) {
            set_next(p, n);
        }
    }

    mark_edge_deleted(e);
}

halfedge_handle halfedge_graph::find_halfedge(vertex_handle v0, vertex_handle v1) const {
    if (halfedges.is_valid(get_halfedge(v0))) {
        for (const auto h : get_halfedges(v0)) {
            if (get_to_vertex(h) == v1) {
                return h;
            }
        }
    }

    return halfedge_handle();
}

edge_handle halfedge_graph::find_edge(vertex_handle v0, vertex_handle v1) const {
    auto h = find_halfedge(v0, v1);
    return halfedges.is_valid(h) ? get_edge(h) : edge_handle();
}

halfedge_graph::vertex_around_vertex_circulator::vertex_around_vertex_circulator(const halfedge_graph *ds,
                                                                                 vertex_handle v) : ds(ds),
                                                                                                    active(false) {
    if (ds) {
        halfedge = ds->get_halfedge(v);
    }
}

bool
halfedge_graph::vertex_around_vertex_circulator::operator==(const vertex_around_vertex_circulator &rhs) const {
    assert(ds);
    assert(ds == rhs.ds);
    return (active && (halfedge == rhs.halfedge));
}

bool
halfedge_graph::vertex_around_vertex_circulator::operator!=(const vertex_around_vertex_circulator &rhs) const {
    return !operator==(rhs);
}

halfedge_graph::vertex_around_vertex_circulator &halfedge_graph::vertex_around_vertex_circulator::operator++() {
    assert(ds);
    //halfedge = ds->rotate_ccw(halfedge);
    auto next = ds->rotate_ccw(halfedge);
    halfedge = ds->halfedges.is_valid(next) ? next : halfedge;
    active = true;
    return *this;
}

halfedge_graph::vertex_around_vertex_circulator &halfedge_graph::vertex_around_vertex_circulator::operator--() {
    assert(ds);
    halfedge = ds->rotate_cw(halfedge);
    return *this;
}

vertex_handle halfedge_graph::vertex_around_vertex_circulator::operator*() const {
    assert(ds);
    return ds->get_to_vertex(halfedge);
}

halfedge_graph::vertex_around_vertex_circulator::operator bool() const {
    return halfedge.is_valid();
}

halfedge_graph::vertex_around_vertex_circulator &halfedge_graph::vertex_around_vertex_circulator::begin() {
    active = !halfedge.is_valid();
    return *this;
}

halfedge_graph::vertex_around_vertex_circulator &halfedge_graph::vertex_around_vertex_circulator::end() {
    active = true;
    return *this;
}

halfedge_graph::vertex_around_vertex_circulator halfedge_graph::get_vertices(vertex_handle v) const {
    return vertex_around_vertex_circulator(this, v);
}

halfedge_graph::halfedge_around_vertex_circulator::halfedge_around_vertex_circulator(const halfedge_graph *ds,
                                                                                     vertex_handle v) : ds(ds),
                                                                                                        active(false) {
    if (ds) {
        halfedge = ds->get_halfedge(v);
    }
}

bool halfedge_graph::halfedge_around_vertex_circulator::operator==(const halfedge_around_vertex_circulator &rhs) const {
    assert(ds);
    assert(ds == rhs.ds);
    return (active && (halfedge == rhs.halfedge));
}

bool halfedge_graph::halfedge_around_vertex_circulator::operator!=(const halfedge_around_vertex_circulator &rhs) const {
    return !operator==(rhs);
}

halfedge_graph::halfedge_around_vertex_circulator &halfedge_graph::halfedge_around_vertex_circulator::operator++() {
    assert(ds);
    auto next = ds->rotate_ccw(halfedge);
    halfedge = ds->halfedges.is_valid(next) ? next : halfedge;
    active = true;
    return *this;
}

halfedge_graph::halfedge_around_vertex_circulator &halfedge_graph::halfedge_around_vertex_circulator::operator--() {
    assert(ds);
    halfedge = ds->rotate_cw(halfedge);
    return *this;
}

halfedge_handle halfedge_graph::halfedge_around_vertex_circulator::operator*() const {
    return halfedge;
}

halfedge_graph::halfedge_around_vertex_circulator::operator bool() const {
    return halfedge.is_valid();
}

halfedge_graph::halfedge_around_vertex_circulator &halfedge_graph::halfedge_around_vertex_circulator::begin() {
    active = !halfedge.is_valid();
    return *this;
}

halfedge_graph::halfedge_around_vertex_circulator &halfedge_graph::halfedge_around_vertex_circulator::end() {
    active = true;
    return *this;
}

halfedge_graph::halfedge_around_vertex_circulator halfedge_graph::get_halfedges(vertex_handle v) const {
    return halfedge_around_vertex_circulator(this, v);
}

std::vector<VectorI<2>> halfedge_graph::get_connectivity() const {
    std::vector<VectorI<2>> connectivity;
    for (const auto e : edges) {
        auto v0 = get_to_vertex(get_halfedge(e, 1));
        auto v1 = get_to_vertex(get_halfedge(e, 0));
        connectivity.push_back({int(v0.idx), int(v1.idx)});
    }
    return connectivity;
}

halfedge_handle halfedge_graph::new_edge(vertex_handle v0, vertex_handle v1) {
    assert(v0 != v1);
    edges.push_back();
    halfedges.push_back();
    halfedges.push_back();

    halfedge_handle h(halfedges.size() - 2);
    halfedge_handle o(halfedges.size() - 1);

    set_vertex(h, v1);
    set_vertex(o, v0);

    assert(halfedges.is_dirty());
    assert(edges.is_dirty());
    return h;
}

void halfedge_graph::mark_edge_deleted(edge_handle e) {
    if (edges_deleted[e]) return;

    edges_deleted[e] = true;
    halfedges_deleted[get_halfedge(e, 0)] = true;
    halfedges_deleted[get_halfedge(e, 1)] = true;
    ++size_edges_deleted;
    ++size_halfedges_deleted;
    ++size_halfedges_deleted;
    edges_deleted.set_dirty();
}

edge_handle halfedge_graph::find_closest_edge(const halfedge_graph::position_t &point) {
    edge_handle closest_yet;
    distance_point3_segment3 distance;
    bcg_scalar_t min_sqr_dist_yet = scalar_max;
    for (const auto e : edges) {
        segment3 seg(positions[get_vertex(e, 0)], positions[get_vertex(e, 1)]);

        bcg_scalar_t sqr_dist = distance(point, seg).sqr_distance;
        if (sqr_dist < min_sqr_dist_yet) {
            min_sqr_dist_yet = sqr_dist;
            closest_yet = e;
        }
    }
    return closest_yet;
}

std::vector<edge_handle> halfedge_graph::find_closest_k_edges(const halfedge_graph::position_t &point, size_t k) {
    using DistIndex = std::pair<bcg_scalar_t, edge_handle>;
    distance_point3_segment3 distance;
    std::vector<DistIndex> closest_k;
    for (const auto e : edges) {
        segment3 seg(positions[get_vertex(e, 0)], positions[get_vertex(e, 1)]);

        bcg_scalar_t sqr_dist = distance(point, seg).sqr_distance;
        if (closest_k.size() < k + 1) {
            closest_k.emplace_back(sqr_dist, e);
            if (closest_k.size() == k) {
                std::sort(closest_k.begin(), closest_k.end(), [](const DistIndex &lhs, const DistIndex &rhs) {
                    return lhs.first < rhs.first;
                });
                break;
            }
            continue;
        }

        if (sqr_dist < closest_k.back().first) {
            closest_k.back() = std::make_pair(sqr_dist, e);
            std::sort(closest_k.begin(), closest_k.end(), [](const DistIndex &lhs, const DistIndex &rhs) {
                return lhs.first < rhs.first;
            });
        }
    }
    std::vector<edge_handle> indices;
    unzip<bcg_scalar_t, edge_handle>(closest_k, nullptr, &indices);
    return indices;
}

std::vector<edge_handle>
halfedge_graph::find_closest_edges_radius(const halfedge_graph::position_t &point, bcg_scalar_t radius) {
    using DistIndex = std::pair<bcg_scalar_t, edge_handle>;
    distance_point3_segment3 distance;
    std::vector<DistIndex> closest;
    for (const auto e : edges) {
        segment3 seg(positions[get_vertex(e, 0)], positions[get_vertex(e, 1)]);

        bcg_scalar_t sqr_dist = distance(point, seg).sqr_distance;
        if (sqr_dist <= radius) {
            closest.emplace_back(sqr_dist, e);
        }
    }
    std::sort(closest.begin(), closest.end(), [](const DistIndex &lhs, const DistIndex &rhs) {
        return lhs.first < rhs.first;
    });
    std::vector<edge_handle> indices;
    unzip<bcg_scalar_t, edge_handle>(closest, nullptr, &indices);
    return indices;
}

edge_handle halfedge_graph::find_closest_edge_in_neighborhood(vertex_handle v,
                                                              const halfedge_graph::position_t &point) {
    distance_point3_segment3 distance;
    edge_handle closest_yet;
    bcg_scalar_t min_dist_yet = scalar_max;
    for (const auto h : get_halfedges(v)) {
        segment3 seg(positions[get_from_vertex(h)], positions[get_to_vertex(h)]);

        bcg_scalar_t sqr_dist = distance(point, seg).sqr_distance;
        if (sqr_dist < min_dist_yet) {
            min_dist_yet = sqr_dist;
            closest_yet = get_edge(h);
        }
    }
    return closest_yet;
}

std::string halfedge_graph::to_string() const{
    std::stringstream stream;
    stream << point_cloud::to_string();
    stream << "graph\n";

    if (has_garbage()) {
        stream << "size_halfedges_deleted: " << size_halfedges_deleted << "\n";
    }
    stream << "halfedge properties:\n" << halfedges << "\n";

    if (has_garbage()) {
        stream << "size_edges_deleted: " << size_edges_deleted << "\n";
    }
    stream << "edge properties:\n" << edges << "\n";
    return stream.str();
}

std::ostream &operator<<(std::ostream &stream, const halfedge_graph &graph){
    stream << graph.to_string();
    return stream;
}

}