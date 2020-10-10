//
// Created by alex on 10.10.20.
//

#include <cassert>
#include "bcg_graph.h"

namespace bcg {

halfedge_graph::halfedge_graph() : point_cloud(),
                                   vconn(vertices.add<vertex_connectivity, 1>("connectivity")),
                                   hconn(halfedges.add<halfedge_connectivity, 1>("connectivity")),
                                   size_halfedges_deleted(0),
                                   size_edges_deleted(0) {

}

void halfedge_graph::assign(const halfedge_graph &other) {
    if (this != &other) {
        point_cloud::assign(other);
        halfedges.remove_all();
        edges.remove_all();
        positions = vertices.get_or_add<position_t, 3>("position");
        vconn = vertices.get_or_add<vertex_connectivity, 1>("connectivity");
        hconn = halfedges.get_or_add<halfedge_connectivity, 1>("connectivity");
        vconn.vector() = other.vconn.vector();
        hconn.vector() = other.hconn.vector();
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
        hconn = halfedges.get_or_add<halfedge_connectivity, 1>("connectivity");
        size_vertices_deleted = other.size_vertices_deleted;
        size_halfedges_deleted = other.size_halfedges_deleted;
        size_edges_deleted = other.size_edges_deleted;
    }
    return *this;
}

bool halfedge_graph::has_garbage() const {
    return size_edges_deleted > 0 || size_edges_deleted > 0 || point_cloud::has_garbage();
}

void halfedge_graph::garbage_collection() {
    if (!has_garbage()) return;
    auto vertices_deleted = vertices.get<bool, 1>("deleted");
    auto halfedges_deleted = halfedges.get<bool, 1>("deleted");
    auto edges_deleted = edges.get<bool, 1>("deleted");

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

}