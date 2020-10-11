//
// Created by alex on 10.10.20.
//

#ifndef BCG_GRAPHICS_BCG_GRAPH_H
#define BCG_GRAPHICS_BCG_GRAPH_H

#include "bcg_point_cloud.h"

namespace bcg {

struct halfedge_graph : public point_cloud {
    struct vertex_connectivity {
        halfedge_handle h;
    };

    struct halfedge_connectivity {
        vertex_handle v;
        halfedge_handle nh;
        halfedge_handle ph;
    };

    property_container halfedges, edges;
    property<vertex_connectivity, 1> vconn;
    property<halfedge_connectivity, 1> hconn;
    size_t size_halfedges_deleted;
    size_t size_edges_deleted;

    halfedge_graph();

    ~halfedge_graph() override = default;

    void assign(const halfedge_graph &other);

    halfedge_graph &operator=(const halfedge_graph &other);

    bool has_garbage() const override;

    void garbage_collection() override;

    bool is_isolated(vertex_handle v) const;

    virtual bool is_boundary(vertex_handle v) const;

    virtual bool is_boundary(halfedge_handle h) const;

    virtual bool is_boundary(edge_handle e) const;

    void delete_vertex(vertex_handle v) override;

    halfedge_handle get_halfedge(vertex_handle v) const;

    void set_halfedge(vertex_handle v, halfedge_handle h);

    vertex_handle get_to_vertex(halfedge_handle h) const;

    void set_vertex(halfedge_handle h, vertex_handle v);

    vertex_handle get_from_vertex(halfedge_handle h) const;

    halfedge_handle get_next(halfedge_handle h) const;

    void set_next(halfedge_handle h, halfedge_handle nh);

    halfedge_handle get_prev(halfedge_handle h) const;

    halfedge_handle get_opposite(halfedge_handle h) const;

    halfedge_handle rotate_cw(halfedge_handle h) const;

    halfedge_handle rotate_ccw(halfedge_handle h) const;

    edge_handle get_edge(halfedge_handle h) const;

    halfedge_handle get_halfedge(edge_handle e, bool i) const;

    vertex_handle get_vertex(edge_handle e, bool i) const;

    size_t get_valence(vertex_handle v) const;

    float get_length(edge_handle e) const;

    point_cloud::position_t get_center(halfedge_handle h) const;

    point_cloud::position_t get_center(edge_handle e) const;

    point_cloud::position_t get_vector(halfedge_handle h) const;

    point_cloud::position_t get_vector(edge_handle e) const;

    halfedge_handle add_edge(vertex_handle v0, vertex_handle v1);

    void remove_edge(edge_handle e);

    halfedge_handle find_halfedge(vertex_handle v0, vertex_handle v1) const;

    edge_handle find_edge(vertex_handle v0, vertex_handle v1) const;

    struct vertex_around_vertex_circulator {
        const halfedge_graph *ds;
        halfedge_handle halfedge;
        bool active;

        explicit vertex_around_vertex_circulator(const halfedge_graph *ds = nullptr, vertex_handle v = vertex_handle());

        inline bool operator==(const vertex_around_vertex_circulator &rhs) const;

        inline bool operator!=(const vertex_around_vertex_circulator &rhs) const;

        inline vertex_around_vertex_circulator &operator++();

        inline vertex_around_vertex_circulator &operator--();

        inline vertex_handle operator*() const;

        inline operator bool() const;

        inline vertex_around_vertex_circulator &begin();

        inline vertex_around_vertex_circulator &end();
    };

    vertex_around_vertex_circulator get_vertices(vertex_handle v) const;

    struct halfedge_around_vertex_circulator {
        const halfedge_graph *ds;
        halfedge_handle halfedge;
        bool active;

        explicit halfedge_around_vertex_circulator(const halfedge_graph *ds = nullptr,
                                                   vertex_handle v = vertex_handle());

        inline bool operator==(const halfedge_around_vertex_circulator &rhs) const;

        inline bool operator!=(const halfedge_around_vertex_circulator &rhs) const;

        inline halfedge_around_vertex_circulator &operator++();

        inline halfedge_around_vertex_circulator &operator--();

        inline halfedge_handle operator*() const;

        inline operator bool() const;

        inline halfedge_around_vertex_circulator &begin();

        inline halfedge_around_vertex_circulator &end();
    };

    halfedge_around_vertex_circulator get_halfedges(vertex_handle v) const;

    std::vector<vec2i> get_connectivity() const;

    halfedge_handle new_edge(vertex_handle v0, vertex_handle v1);
};


edge_handle find_closest_edge(const halfedge_graph &graph, const halfedge_graph::position_t &point);

std::vector<edge_handle> find_closest_k_edges(const halfedge_graph &graph, const halfedge_graph::position_t &point, size_t k) ;

std::vector<edge_handle> find_closest_edges_radius(const halfedge_graph &graph, const halfedge_graph::position_t &point, float radius);

edge_handle find_closest_edge_in_neighborhood(const halfedge_graph &graph, vertex_handle v, const halfedge_graph::position_t &point);


}

#endif //BCG_GRAPHICS_BCG_GRAPH_H
